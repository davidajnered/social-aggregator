<?php
class SocialAggregator {

  // Variables
  private static $instance;
  private $plugins;
  private $plugin_instances;
  private $subdir = FALSE;

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
    foreach($this->plugins as $plugin) {
      // Create and instance for all the plugins
      $plugin_instance = new $plugin;
      // Run plugin config
      $plugin_instance->config();
      // Store them in an array
      $this->plugin_instances[$plugin_instance->getMachineName()] = $plugin_instance;

    }
  }
  
  /**
   * Get the data from plugins api implementation
   */
  public function fetchData() {
    $stored_values = get_option('social_aggregator_stored_values', $stored_values);
    $aggregated_data = array();

    // add var that decide how often we collect new data. We don't need to do it on every request
    foreach($this->plugin_instances as $plugin_instance) {
      // Get all the stored values (username, password and stuff like that) related to this plugin
      $plugin_values = $stored_values[$plugin_instance->getMachineName()];
      // Call plugin with values
      $plugin_data = $plugin_instance->data($plugin_values);
      if(!is_array($plugin_data)) {
        // Error: plugin returned no data
        return;
      }
      // Write data to database
      foreach($plugin_data as $data) {
        self::writeToDatabase($plugin_instance->getMachineName(), $plugin_instance->timestamp($data), $data);
      }
    }
    return $aggregated_data;
  }
  
  public function data() {
    global $wpdb;
    $table = $wpdb->prefix . "social_aggregator";
    $query = "SELECT * FROM " . $table . " ORDER BY date ASC LIMIT 0,10";
    if ($wpdb->query($query) === FALSE) {
      return FALSE;
    }
    else {
      $results = $wpdb->get_results($query);
      foreach($results as $result) {
        $content = $result->content;
        $content = unserialize($content);

        // load the correct plugin instance and call render with $content
        $plugin_instance = $this->plugin_instances[$result->service];
        $plugin_instance->render($content);
      }
    }
  }

  /**
   * Store plugin data
   */
  public function writeToDatabase($plugin, $timestamp, $data) {
    $data = self::sanitizeData($data);
    global $wpdb;
    $table = $wpdb->prefix . "social_aggregator";
    
    $id = md5($plugin . $timestamp);
    $data = mysql_real_escape_string($data);
    $query = "INSERT IGNORE INTO " . $table . " (id, service, date, content) VALUES ('$id', '$plugin', $timestamp, '$data')";
    if ($wpdb->query($query) === FALSE) {
      return FALSE;
    } else {
      return $wpdb->get_results($query);
    }
  }
  
  public function sanitizeData($data) {
    if(is_array($data) || is_object($data)) {
      $data = serialize($data);
    }
    return $data;
  }

  /**
   * Scan through the plugin folder for available plugins
   */
  private function getPlugins() {
    $dir = dirname(__FILE__) . '/plugins';
    $this->checkDir($dir);
  }
  
  /**
   * Recursive function searching for plugins
   */
  private function checkDir($dir) {
    $ignore = array('.', '..', 'SocialAggregatorPlugin.inc');
    if(is_dir($dir)) {
      if ($handle = opendir($dir)) {
        while (false !== ($file = readdir($handle))) {
          if(!in_array($file, $ignore)) {
            $file_path = $dir . '/' . $file;
            if(is_dir($file_path) && !$this->subdir) {
              $this->subdir = TRUE;
              $this->checkDir($file_path);
              break;
            } else {
              $result = $this->checkFile($file, $file_path);
              if($result && $this->subdir) {
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
  */
  private function checkFile($file, $file_path) {
    if(strpos($file, 'SocialAggregator') === 0) {
      require_once($file_path);
      $this->plugins[] = str_replace('.inc', '', $file);
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