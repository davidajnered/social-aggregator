<?php
class SocialAggregator {

  // Variables
  private $pluginClasses;
  private $pluginInstances;

  /**
   * Singleton construct method
   */
  public function SocialAggregator() {
    self::getPlugins();
    self::setupPluginOptions();
    self::fetchData();
  }

  /**
   * This is a setup function
   * We need to get data from the plugin like specifications for the API call to the service
   * This is stored in an the option array so we can display the correct fields on the settings page
   */
  public function setupPluginOptions() {
    foreach($this->pluginClasses as $plugin) {
      // Create and instance all the present plugins
      $instance = new SocialAggregatorPlugin();
      $instance->init($plugin);
      // Get the configuration array from the plugin and store it as an option for easy access
      //$instance->config();
      $this->pluginInstances[$instance->getClassName()] = $instance;
    }
  }

  /**
   * Get the data from plugins api implementation
   */
  public function fetchData() {
    $options = get_option('social_aggregator_stored_values', array());
    // Add a variable that decide how often we collect new data. We don't need to do it on every request
    foreach($this->pluginInstances as $plugin) {
      // Get all the stored values (username, password and stuff like that) related to this plugin
      $values = $options[$plugin->getMachineName()];
      // Call plugin with values
      $data = $plugin->data($values);
      if(is_array($data)) {
        // Loop through all the posts and write them to the database
        foreach($data as $post) {
          self::dbWrite($plugin, $post);
        }
      }
      else {
        // Unset instance for this request
        unset($this->pluginInstances[$plugin->getClassName()]);
      }
    }
  }

  /**
   * Write plugin data to database
   * @param $plugin The plugin instance
   * @param $data The post data to be inserted in the database
   * @return If query returns data we return the result object, else false
   */
  public function dbWrite($plugin, $post) {
    global $wpdb;
    $plugin_name = $plugin->getClassName();
    $table = $wpdb->prefix . "social_aggregator";
    $timestamp = $plugin->timestamp($post);
    $id = md5($plugin_name . $timestamp);
    $post = mysql_real_escape_string(serialize($post));
    $query = "INSERT IGNORE INTO " . $table . " (id, plugin, date, content) VALUES ('$id', '$plugin_name', $timestamp, '$post')";
    if ($wpdb->query($query) === FALSE) {
      return FALSE;
    }
    else {
      return $wpdb->get_results($query);
    }
  }

  /**
   * Get data from database and render to view
   * @return return false if render fails
   */
  public function data() {
    global $wpdb;
    $table = $wpdb->prefix . "social_aggregator";
    $query = "SELECT * FROM " . $table . " ORDER BY date DESC LIMIT 0,30";
    if ($wpdb->query($query) === FALSE) {
      return FALSE;
    }
    else {
      $results = $wpdb->get_results($query);
      foreach($results as $result) {
        $content = $result->content;
        $content = unserialize($content);
        // load the correct plugin instance and call render with $content
        $pluginInstance = $this->pluginInstances[$result->plugin];
        $pluginInstance->render($content);
      }
    }
  }

  /**
   * Scan through the plugin folder for available plugins
   */
  private function getPlugins() {
    $dir = dirname(__FILE__) . '/plugins';
    self::verifyDirectory($dir);
  }

  /**
   * Recursive method that search for plugins
   * @param $dir the directory to scan
   */
  private function verifyDirectory($dir) {
    static $subdir;
    $ignore = array('.', '..', 'SocialAggregatorPlugin.inc');
    if(is_dir($dir)) {
      if ($handle = opendir($dir)) {
        while (false !== ($file = readdir($handle))) {
          if(!in_array($file, $ignore)) {
            $file_path = $dir . '/' . $file;
            if(is_dir($file_path) && !$this->subdir) {
              $subdir = TRUE;
              self::verifyDirectory($file_path);
              break;
            } else {
              $result = self::verifyFile($file, $file_path);
              if($result && $subdir) {
                break;
              }
            }
          }
        }
      }
    }
  }

 /**
  * Validate file
  * @return true if file is a valid plugin, false for everything else
  * @param $file the file name
  * @param $file_path the file path
  */
  private function verifyFile($file, $file_path) {
    if(strpos($file, 'SocialAggregator') === 0) {
      require_once($file_path);
      $this->pluginClasses[] = str_replace('.inc', '', $file);
      // Load stored values and set the present once as active, the disabled once as passiv (1's and 0's)
      return TRUE;
    }
  }

  public function adminForm() {
    foreach($this->pluginInstances as $plugin) {
      $fields = $plugin->config();
      print '<h3>' . $plugin->getPluginName() . '</h3>';
      foreach($fields as $key => $field) : ?>
        <p>
        <?php
          if(!is_int($key)) {
            $deep_fields = $field;
            foreach($deep_fields as $field) {
              self::adminFormElement($field);
            }
          }
          else {
            self::adminFormElement($field);
          }
        ?>
        </p>
      <?php endforeach; ?>
      <input type="hidden" name="social_aggregator_submit" value="y">
      <input class="button-primary" type="submit" name="submit" value="Save" />
      <?php
    }
  }
  public function adminFormSubmit() {
    // $plugins = get_option('social_aggregator_plugins');
    // Is form submitted?
    if(isset($_POST['social_aggregator_submit']) && $_POST['social_aggregator_submit'] == 'y') {
      $ignore = array('social_aggregator_submit', 'Submit');
      error_log(var_export($_POST, TRUE));
      foreach($_POST as $field => $value) {
        if(!in_array($field, $ignore)) {
          // Explode name to get [0] = plugin machine name, [1] = field name
          // We only use this for plugin fields where we have to know what plugin
          // can we use social_aggregator_get_field_value here instead?
          $parts = explode(':', $field);
          if(isset($parts[1])) {
            $stored_values[$parts[0]][$parts[1]] = $value;
          }
          else {
            $stored_values[$field] = $value;
          }
        }
      }
    }
    //error_log(var_export($stored_values, TRUE));
    update_option('social_aggregator_values', $stored_values); ?>
    <div class="updated"><p><strong><?php _e('settings saved.', 'menu-test' ); ?></strong></p></div>
  <?php }
  
  private function adminFormElement($field) { ?>
    <label for="<?php print $field['name']; ?>">
      <?php print $field['title'] ? $field['title'] : ucfirst($field['name']); ?>
    </label>
    <input type="<?php print $field['type']; ?>"
           name="<?php print $field['name']?>"
           <?php self::adminFormElementStoredValue($field); ?> />
  <?php }
  
  private function adminFormElementStoredValue($field) {
    $values = get_option('social_aggregator_values', '');
    if($field['type'] == 'checkbox') {
      $values[$field['name']];
      error_log(var_export($values[$field['name']], TRUE));
      print ' value="' . $field['value'] . '"';
    }
    else {
      print ' value="' . $values[$field['name']] . '"';
    }
  }
} // End of class

/**
 * Autoload classes in the plugin folder
 */
function __autoload($class) {
  $file = dirname(__FILE__) . '/plugins/' . $class . '.inc';
  if(file_exists($file)) {
    require_once($file);
  }
}