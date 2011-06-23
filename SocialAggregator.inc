<?php
class SocialAggregator {

  // Variables
  private static $instance;
  private $plugins;
  private $plugin_objects;

  /**
   * Singleton construct method
   */
  public function SocialAggregator() {
    $this->setupPluginOptions();
  }

  /**
   * This is a setup function
   * We need to get data from the plugin like specifications for the API call to the service
   * This is stored in an the option array so we can display the correct fields on the settings page
   */
  public function setupPluginOptions() {
    $this->getPlugins();
    foreach($this->plugins as $plugin) {
      $plugin_object = new $plugin;
      $this->plugin_objects[] = &$plugin_object;
      $plugin_object->config();
    }
  }
  
  /**
   * Get the data from plugins api implementation
   */
  public function getPluginData() {
    $stored_values = get_option('social_aggregator_stored_values', $stored_values);
    foreach($this->plugin_objects as $plugin_object) {
      // Get all the stored values related to this plugin
      $plugin_values = $stored_values[$plugin_object->getMachineName()];
      $data = $plugin_object->data($plugin_values);
    }
    return $data;
  }

  /**
   * Scan through the plugin folder for available plugins
   */
  private function getPlugins() {
    $plugins = array();
    if ($handle = opendir(dirname(__FILE__) . '/plugins')) {
      $ignore = array('.', '..', 'SocialAggregatorPlugin.inc');
      while (false !== ($file = readdir($handle))) {
        if(!in_array($file, $ignore)) {
          $plugins[] = str_replace('.inc', '', $file);
        }
      }
      closedir($handle);
    }
    $this->plugins = $plugins;  
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