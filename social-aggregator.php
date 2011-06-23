<?php 
/*
 * Plugin Name: Social Aggregator
 * Version: 0.1
 * Plugin URI: http://davidajnered.com/social-aggregator/
 * Description: Aggregated feed of all your social
 * Author: David Ajnered
 * Author URI: http://davidajnered.com/
 * Text Domain: social-aggregator
 */
 
/*
  Hur den är tänkt att fungera:
  Social aggregator tillhandahåller ett ramverk för att skapa ett aggregerat flöde. För att lägga till ett nätverk i flödet måste det skapas en plugin.
  Pluginen måste:
  * Tillhandahålla en funktion för att kunna hämta data från tjänsten
  * Hook
  
  To think about
  Hur anropar man en okänd funktion i wordpress?
  Cachning!
*/

add_action('init', 'social_aggregator_init');
function social_aggregator_init() {
  require_once('SocialAggregator.inc');
  $collector = new SocialAggregator();
  //$collector->setupPlugins();
}

add_action('admin_menu', 'social_aggregator_settings_menu');
function social_aggregator_settings_menu() {
  add_menu_page( 'Social Aggregator', 'Social Aggregator', 'edit_posts', 'social-aggregator', 'social_aggregator_settings_page' );
  add_options_page('Test Settings', 'Test Settings', 'manage_options', 'testsettings', 'mt_settings_page');
}

function social_aggregator_settings_page() {
  if (!current_user_can('manage_options')) {
    wp_die( __('You do not have sufficient permissions to access this page.') );
  }

  // variables for the field and option names 
  $option = 'social_aggregator_plugins';
  $hidden_submit_field = 'sa_submit_hidden';
  //$data_field_name = 'mt_favorite_color';

  // Show availabe networks. Store an array with networks in options.
  // Network is available if there is a plugin for it
  // Store array with settings for the network

  $plugins = get_option($option);
  error_log(var_export('options', TRUE));
  error_log(var_export($plugins, TRUE));
  // Is form submitted?
  if( isset($_POST[ $hidden_submit_field ]) && $_POST[ $hidden_submit_field ] == 'Y' ) {
    //$o = $_POST[ $data_field_name ];
    //$network_array = array();
    //update_option( $opt_name, $opt_val ); ?>
    <div class="updated"><p><strong><?php _e('settings saved.', 'menu-test' ); ?></strong></p></div>
    <?php
  }
?>

<div class="wrap">
  <h2>Social Aggregator Settings</h2>
  <form name="networks" method="post" action="">
    <input type="hidden" name="<?php echo $hidden_submit_field; ?>" value="Y">
    <h3>Plugins:</h3>
    <?php foreach($plugins as $plugin) : ?>
      <fieldset>
        <legend><?php print $plugin['plugin name']; ?></legend>
        <?php foreach($plugin['user config'] as $name => $field) : ?>
          <!-- Move this to a separate function -->
          <p>
            <label for="<?php print $name; ?>"><?php print $name; ?></label>
            <input type="<?php print $field['type']; ?>" name="<?php print $name; ?>">
          </p>
        <?php endforeach; ?>
      </fieldset>
    <?php endforeach; ?>

    <p class="submit">
      <input type="submit" name="Submit" class="button-primary" value="Save Changes" />
    </p>
  </form>
</div>

<?php
}

function social_aggregator_get_formated_field($data) {
  return $field;
}

function the_social_feed() {
  return $return;
}