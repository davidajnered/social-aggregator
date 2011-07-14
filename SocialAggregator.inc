<?php
class SocialAggregator {

  // Variables
  private $plugin_classes;
  private $plugin_instances;

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
    $configuration = array();
    foreach($this->plugin_classes as $plugin) {
      // Create and instance all the present plugins
      $instance = new SocialAggregatorPlugin();
      $instance->init($plugin);
      // Get the configuration array from the plugin and store it as an option for easy access
      $configuration += $instance->config();
      $this->plugin_instances[$instance->getClassName()] = $instance;
    }
    update_option('social_aggregator_plugins', $configuration);
  }

  /**
   * Get the data from plugins api implementation
   */
  public function fetchData() {
    $options = get_option('social_aggregator_stored_values', array());
    // Add a variable that decide how often we collect new data. We don't need to do it on every request
    foreach($this->plugin_instances as $plugin) {
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
        // unset($this->plugin_instances[get_class($plugin_instance)]);
      }
    }
  }

  /**
   * Write plugin data to database
   * @param $plugin_instance The plugin instance
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
        $plugin_instance = $this->plugin_instances[$result->plugin];
        $plugin_instance->render($content);
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
      $this->plugin_classes[] = str_replace('.inc', '', $file);
      // Load stored values and set the present once as active, the disabled once as passiv (1's and 0's)
      return TRUE;
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