<?php
class SocialAggregator {

  // Variables
  private static $instance;
  private $plugins;
  private $plugin_objects;
  private $subdir = FALSE;

  /**
   * Singleton construct method
   */
  public function SocialAggregator() {
    $this->getPlugins();
    $this->setupPluginOptions();
  }

  /**
   * This is a setup function
   * We need to get data from the plugin like specifications for the API call to the service
   * This is stored in an the option array so we can display the correct fields on the settings page
   */
  public function setupPluginOptions() {
    foreach($this->plugins as $plugin) {
      $plugin_instance = new $plugin;
      $this->plugin_instances[] = $plugin_instance;
      $plugin_instance->config();
    }
  }
  
  /**
   * Get the data from plugins api implementation
   */
  public function getPluginData() {
    $stored_values = get_option('social_aggregator_stored_values', $stored_values);
    $aggregated_data = array();
    foreach($this->plugin_instances as $plugin_instance) {
      // Get all the stored values related to this plugin
      $plugin_values = $stored_values[$plugin_instance->getMachineName()];
      $aggregated_data = $plugin_instance->data($plugin_values);
    }
    return $aggregated_data;
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