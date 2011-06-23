<?php
class SocialAggregator {

  // Variables
  private static $instance;

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
    // ladda options som vi ska skicka till data-methoden
    $plugins = $this->getPlugins();
    foreach($plugins as $plugin) {
      $object = new $plugin;
      $config = $object->config();
    }
  }
  
  /**
   * Get the data from the plugin api implementation
   */
  public function getPluginData() {
    
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
    return $plugins;  
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