<?php
class SocialAggregator {

  // Variables
  private $pluginClasses;
  private $pluginInstances;

  /**
   * Singleton construct method
   */
  public function SocialAggregator() {
    self::scanPluginDir();
    self::initPlugins();
    self::fetchData();
  }

  /**
   * This is a setup function
   * We need to get data from the plugin like specifications for the API call to the service
   * This is stored in an the option array so we can display the correct fields on the settings page
   */
  public function initPlugins() {
    foreach($this->pluginClasses as $plugin) {
      // Create and instance all the present plugins
      $instance = new SocialAggregatorPlugin();
      $instance->init($plugin);
      $this->pluginInstances[$instance->getClassName()] = $instance;
    }
    // We check if settings form is submitted early on so we don't have to load data twice due to the redirect
    self::adminFormSubmit();
  }

  /**
   * Get the data from plugins api implementation
   */
  public function fetchData() {
    // Add a variable that decide how often we collect new data. We don't need to do it on every request
    foreach($this->pluginInstances as $plugin) {
      if($plugin->isActive()) {
        $values = get_option('social_aggregator_' . $plugin->getMachineName() . '_values', array());
        $data = $plugin->data($values);
        if(is_array($data)) {
          // Loop through all the posts and write them to the database
          foreach($data as $post) {
            self::dbWrite($plugin, $post);
          }
        }
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
    // This one needs improvement. We need to check what plugins are active before we do the query.
    // Only select posts from active plugins
    global $wpdb;
    $table = $wpdb->prefix . "social_aggregator";
    
    $active_plugins = self::getActivePlugins();
    error_log(var_export($active_plugins, TRUE));
    $loop = 0;
    foreach($active_plugins as $plugin) {
      if($loop == 0) {
         $where = ' WHERE';
      } else {
        $where .= ' AND';
      }
      $where .= " plugin = '" . $plugin['class'] . "'";
    }
    $query = "SELECT * FROM " . $table . $where . " ORDER BY date DESC LIMIT 0,30";
    error_log(var_export($query, TRUE));
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
        if(is_object($pluginInstance)) {
          $pluginInstance->render($content);
        }
      }
    }
  }

  /**
   * Scan through the plugin folder for available plugins
   */
  private function scanPluginDir() {
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
    foreach($this->pluginInstances as $plugin) :
      $fields = $plugin->config(); ?>
      <div class="social_aggregator_plugin_wrapper" id="social_aggregator_<?php print $plugin->getMachineName(); ?>_wrapper">
        <h3><?php print $plugin->getPluginName(); ?></h3>
        <?php foreach($fields as $key => $field) : ?>
          <?php
            if(!is_int($key)) : ?>
              <h4><?php print ucfirst($key); ?></h4>
              <?php $deep_fields = $field;
              foreach($deep_fields as $field) {
                self::adminFormElement($field, $plugin->getMachineName());
              }
            else :
              self::adminFormElement($field, $plugin->getMachineName());
            endif;
          ?>
        <?php endforeach; ?>
        </div>
    <?php endforeach; ?>
    <input type="hidden" name="social_aggregator_submit" value="y" />
    <input class="button-primary" type="submit" name="submit" value="Save" />
  <?php
  }

  public function adminFormSubmit() {
    if(isset($_POST['social_aggregator_submit']) && $_POST['social_aggregator_submit'] == 'y') {
      update_option('social_aggregator_submitted', TRUE);
      $ignore = array('social_aggregator_submit', 'Submit');
      foreach($_POST as $field => $value) {
        if(!in_array($field, $ignore)) {
          $parts = explode(':', $field);
          // Plugin settings
          if(isset($parts[1])) {
            $plugin = $parts[0];
            $values[$parts[1]] = $value;
            $plugin = self::getPluginFromName($plugin);
            if(is_object($plugin)) {
              $plugin->storeValues($values);
            }
          }
          // Global settings
          else {
            $values[$field] = $value;
            update_option('social_aggregator_values', $values);
          }
        }
      }
      self::redirect();
      ?>
      <div class="updated"><p><strong><?php _e('settings saved.', 'menu-test' ); ?></strong></p></div>
      <?php
    }
  }

  private function redirect() {
    $url = 'http';
    if ($_SERVER["HTTPS"] == "on") {
      $url .= "s";
    }
    $url .= "://";
    if ($_SERVER["SERVER_PORT"] != "80") {
      $url .= $_SERVER["SERVER_NAME"] . ":" . $_SERVER["SERVER_PORT"] . $_SERVER["REQUEST_URI"];
    }
    else {
      $url .= $_SERVER["SERVER_NAME"] . $_SERVER["REQUEST_URI"];
    }
    print '
      <script type="text/javascript">
        <!--
        window.location = "' . $url . '"
        //-->
      </script>';
  }

  private function adminFormElement($field, $plugin = NULL) { ?>
    <p class="<?php print $field['name'] . '_wrapper'; ?>" id="<?php print $plugin . '_' . $field['name'] . '_wrapper'; ?>">
      <label for="<?php print $field['name']; ?>"><?php print $field['title'] ? $field['title'] : ucfirst($field['name']); ?></label>
      <?php
      $type = self::adminFormElementType($field);
      $name = self::adminFormFormatName($field, $plugin);
      $stored_values = self::adminFormElementStoredValue($field, $plugin); ?>
      <input <?php print $type . $name . $stored_values; ?> />
    </p>
  <?php }

  private function adminFormElementType($field) {
    return 'type="' . $field['type'] .'"';
  }

  private function adminFormFormatName($field, $plugin) {
    $name = $field['name'];
    if($plugin != NULL) {
      $name = $plugin . ':' . $name;
    }
    if($field['type'] == 'checkbox') {
      $name .= '[' . $field['value'] . ']';
    }
    return 'name="'. $name . '" class="'. $field['name'] . '" id="' . $plugin . '_' . $field['name'] . '"';
  }

  private function adminFormElementStoredValue($field, $plugin) {
    $values = get_option('social_aggregator_' . $plugin . '_values', '');
    if($plugin == NULL) {
      $values = get_option('social_aggregator_values', '');
    }
    $value = $values[$field['name']];
    if($plugin != NULL && isset($values[$plugin])) {
      $value = $values[$plugin][$field['name']];
    }

    if($field['type'] == 'checkbox') {
      $stored_values = ' value="' . $field['value'] . '"';
      if($value == $field['value'] || (is_array($value) && in_array($field['value'], $value))) {
        $stored_values .= ' checked';
      }
    }
    else {
      $stored_values = ' value="' . $value . '"';
    }
    return $stored_values;
  }
  
  private function getPluginFromName($name) {
    foreach($this->pluginInstances as $plugin) {
      if($plugin->getMachineName() == $name || $plugin->getPluginName() == $name) {
        return $plugin;
      }
    }
    return FALSE;
  }
  
  private function getActivePlugins() {
    $active = array();
    foreach($this->pluginInstances as $plugin) {
      if($plugin->isActive()) {
        $active_plugin = array(
          'name' => $plugin->getPluginName(),
          'class' => $plugin->getClassName(),
        );
        $active[$plugin->getMachineName()] = $active_plugin;
      }
    }
    return $active;
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