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
  private $values;

  /**
   * Constructor
   * @param $plugin class name of plugin to be created
   * @return plugin instance
   */
  public function init($plugin) {
    $this->instance = new $plugin;
    // Store values in plugin
    $this->values = get_option('social_aggregator_' . self::getMachineName() . '_values', FALSE);
    return $this->instance;
  }

  /**
   * Get data from the plugin
   */
  public function data($params = NULL) {
    $params = func_get_args();
    unset($params[0]['plugin-active']);
    error_log(var_export($params[0], TRUE));
    $data = call_user_func_array(array($this->instance, 'data'), $params[0]);
    return $data;
  }

  /**
   * Get the configuration array from the plugin
   * @return array defining configuration needed for the plugin
   */
  public function config() {
    $config = $this->instance->config();
    $config[] = array(
      'title'   => 'Active',
      'name'    => 'plugin-active',
      'value'   => 'active',
      'type'    => 'checkbox',
    );
    return $config;
  }

  /**
  * Render data to view. If there's a override hook implemented in the themes function.php we call it,
  * else we use the plugins render method
  * @param $data the data
  */
  public function render($data) {
    $func = strtolower(self::getMachineName()) . '_render';
    if(function_exists($func)) {
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

  /**
   * Get a name as machine name with no spaces and uppercase characters
   * @param @name the name to be converted
   */
  public function getMachineName($name = NULL) {
    if($name != NULL) {
      return strtolower(str_replace(' ', '_', $name));
    }
    return strtolower(str_replace(' ', '_', $this->instance->pluginName));
  }

  public function getValues() {
    return $this->values;
  }

  public function getValue($name = NULL) {
    if($name != NULL && isset($this->values[$name])) {
      return $this->values[$name];
    }
    return FALSE;
  }

  public function storeValues($values) {
    update_option('social_aggregator_' . self::getMachineName() . '_values', $values);
  }

  public function isActive() {
    if(isset($this->values['plugin-active'])) {
      return TRUE;
    }
    return FALSE;
  }
}