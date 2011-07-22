<?php

class SocialAggregatorAdmin {

  private $plugins;

  public function setPlugins($plugins) {
    $this->plugins = $plugins;
  }

  /**
   * Prints admin form
   */
  public function form() {
    foreach($this->plugins as $plugin) :
      $fields = $plugin->_config(); ?>
      <div class="social_aggregator_plugin_wrapper" id="social_aggregator_<?php print $plugin->getMachineName(); ?>_wrapper">
        <h3><?php print $plugin->getPluginName(); ?></h3>
        <?php foreach($fields as $key => $field) {
          self::adminFormElement($field, $plugin->getMachineName());
        } ?>
      </div>
    <?php endforeach; ?>
    <input type="hidden" name="social_aggregator_submit" value="y" />
    <input class="button-primary" type="submit" name="submit" value="Save" />
  <?php
  }

  /**
   * Form submit
   */
  public function submit() {
    error_log(var_export('submit', TRUE));
    if(isset($_POST['social_aggregator_submit']) && $_POST['social_aggregator_submit'] == 'y') {
      update_option('social_aggregator_submitted', TRUE);
      $ignore = array('social_aggregator_submit', 'Submit');
      $plugin_values = array();
      $global_values = array();
      foreach($_POST as $field => $value) {
        if(!in_array($field, $ignore)) {
          // Loop through the values.
          // Use config array to get the relevant once
          $res = preg_grep("/^plugin_([0-9a-zA-Z])_/", array($field));
          error_log(var_export($res, TRUE));
          $parts = explode(':', $field);
          // Plugin settings
          if(isset($parts[1])) {
            $plugin_values[$parts[0]][$parts[1]] = $value;
          }
          // Global settings
          else {
            $global_values[$field] = $value;
          }
        }
      }
      // Store new values
      foreach($plugin_values as $plugin_name => $values) {
        $plugin = self::getPluginFromName($plugin_name);
        if(is_object($plugin)) {
          $plugin->storeValues($values);
        }
      }
      update_option('social_aggregator_values', $global_values);
      self::redirect();
      ?>
      <div class="updated"><p><strong><?php _e('settings saved.', 'menu-test' ); ?></strong></p></div>
      <?php
    }
  }

  private function getPluginFromField($field) {
    
  }

  /**
   * Build input string
   */
  private function adminFormElement($field, $plugin = NULL) {
    print '<p>';

    // Setup vars
    $title = $field['title'] ? $field['title'] : ucfirst($field['name']);
    // formatted field
    if($field['type'] == 'checkbox') {
      foreach($field['value'] as $value) {
        $field_values['name'] = $value['name'];
        $field_values['title'] = $value['title'];
        $field_values['input_type'] = self::adminFormElementType($field);
        $field_values['input_name'] = self::adminFormFormatName($field, $plugin);
        $field_values['input_values'] = self::adminFormElementStoredValue($field, $plugin, $value);
        self::printFormElement($field_values);
      }
    }
    else {
      $field_values['name'] = $field['name'];
      $field_values['title'] = $field['title'];
      $field_values['input_type'] = self::adminFormElementType($field);
      $field_values['input_name'] = self::adminFormFormatName($field, $plugin);
      $field_values['input_values'] = self::adminFormElementStoredValue($field, $plugin);
      self::printFormElement($field_values);
    }

    print '</p>';
  }

  /**
   * Print element
   */
  private function printFormElement($field) { ?>
      <label for="<?php print $field['name']; ?>"><?php print $field['title']; ?></label>
      <input <?php print $field['input_type'] . $field['input_name'] . $field['input_values']; ?> />
  <?php }

  /**
   * Remove?
   */
  private function adminFormElementType($field) {
    return 'type="' . $field['type'] .'"';
  }

  /**
   * Remove?
   */
  private function adminFormFormatName($field, $plugin) {
    $name = $field['name'];
    if($plugin != NULL) {
      $name = 'plugin_' . $plugin . '_' . $name;
    }
    if($field['type'] == 'checkbox') {
      $name .= '[' . $field['value'] . ']';
    }
    return 'name="'. $name . '" class="'. $field['name'] . '" id="' . $plugin . '_' . $field['name'] . '"';
  }

  /**
   * Get stored values for the field
   */
  private function adminFormElementStoredValue($field, $plugin, $value = '') {
    $values = get_option('social_aggregator_' . $plugin . '_values', '');
    if($plugin == NULL) {
      $values = get_option('social_aggregator_values', '');
    }
    $value = $values[$field['name']];
    if($plugin != NULL && isset($values[$plugin])) {
      $value = $values[$plugin][$field['name']];
    }

    if($field['type'] == 'checkbox' && $value != '') {
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

  /**
   * Redirect user to avoid form resubmission
   * Done early on so user should not notice 
   */
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
    // Redirect to avoid form resubmission
    print '
      <script type="text/javascript">
        <!--
        window.location = "' . $url . '"
        //-->
      </script>';
  }

}