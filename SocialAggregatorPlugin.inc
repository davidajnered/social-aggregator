<?php
/**
 * SocialAggregatorPlugin is a wrapper class for plugins.
 * It's purpose is to hide logic from developers looking to extending this plugin
 * One important function for this wrapper is to add hooks that enables front end designers
 * to override the render method with a custom function in function.php
 */

// Delete this somehow
require_once('SocialAggregatorPlugin.interface');
class SocialAggregatorPlugin implements SocialAggregatorPluginInterface {

  private $instance;
  private $options;

  /**
   * Constructor
   * @param $plugin class name of plugin to be created
   * @return plugin instance
   */
  public function init($plugin) {
    $this->instance = new $plugin;
    return $this->instance;
  }

  /**
   * Get data from the plugin
   */
  public function data($params = NULL) {
    $params = func_get_args();
    return $this->instance->data($params[0]);
  }

  /**
   * Get the configuration array from the plugin
   * @return array defining configuration needed for the plugin
   */
  public function config() {
    return $this->instance->config();
    // return self::processConfig($this->options);
  }

  /**
   * Fill config array with some default fields and values
   * @param $config
   * @return array defining configuration needed for the plugin
   */
  public function processConfig($config) {
    // Add machine name for the plugin and all the fields
    $config['machine name'] = self::getMachineName();
    foreach($config['user config'] as $name => $field) {
      $config['user config'][$name]['machine name'] = self::getMachineName($name);
    }

    $configuration[$config['plugin name']] = $config;
    return $configuration;
  }

  /**
  * Render data to view. If there's a override hook implemented in the themes function.php we call it,
  * else we use the plugins render method
  * @param $data the data
  */
  public function render($data) {
    $func = strtolower($this->options['plugin name']) . '_render';
    if($func != NULL && function_exists($func)) {
      call_user_func($func, $data);
    }
    else {
      $this->instance->render($data);
    }
  }

  /**
  * Get the time for a post as a unix timestamp
  * This method have to be implemented by every plugin
  * since the framework doesn't know the structure of the data
  * @param $data the data
  * @param a unix timestamp formatted date
  */
  public function timestamp($data) {
    return $this->instance->timestamp($data);
  }

  /**
   * Get a name as machine name with no spaces and uppercase characters
   * @param @name the name to be converted
   */
  public function getMachineName($name = NULL) {
    if($name != NULL) {
       return strtolower(str_replace(' ', '_', $name));
    }
    return strtolower(str_replace(' ', '_', $this->options['plugin name']));
  }

  /**
   * Get the class name of the plugin object
   * @return class name
   */
  public function getClassName() {
    return get_class($this->instance);
  }

  /**
   * Return plugin name
   */
  public function getPluginName() {
    return $this->instance->pluginName;
  }
}