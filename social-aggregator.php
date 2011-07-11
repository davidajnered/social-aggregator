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

$social_aggregator;

add_action('init', 'social_aggregator_init');
function social_aggregator_init() {
  require_once('SocialAggregator.inc');
  require_once('SocialAggregatorPlugin.inc');
  global $social_aggregator;
  $social_aggregator = new SocialAggregator();
}

add_action('wp_head', 'social_aggregator_head');
function social_aggregator_head() {
  $path = get_bloginfo('wpurl') . '/wp-content/plugins/social-aggregator/social-aggregator.css';
  echo '<link rel="stylesheet" type="text/css" href="' . $path . '">';
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
  $hidden_submit_field = 'sa_submit_hidden';

  // Show availabe networks. Store an array with networks in options.
  // Network is available if there is a plugin for it
  // Store array with settings for the network

  $plugins = get_option('social_aggregator_plugins');
  // Is form submitted?
  if( isset($_POST[ $hidden_submit_field ]) && $_POST[ $hidden_submit_field ] == 'Y' ) {
    $ignore = array('sa_submit_hidden', 'Submit');
    foreach($_POST as $name => $value) {
      if(!in_array($name, $ignore)) {
        // Explode name to get [0] = plugin machine name, [1] = field name
        $names = explode(':', $name);
        $stored_values[$names[0]][$names[1]] = $value;
      }
    }
    update_option('social_aggregator_stored_values', $stored_values); ?>
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
        <?php foreach($plugin['user config'] as $name => $field) {
          social_aggregator_get_formated_field($plugin, $name, $field);
        } ?>
      </fieldset>
    <?php endforeach; ?>

    <p class="submit">
      <input type="submit" name="Submit" class="button-primary" value="Save Changes" />
    </p>
  </form>
</div>

<?php
}

function social_aggregator_get_formated_field($plugin, $name, $field) {
  $stored_values = get_option('social_aggregator_stored_values');
  $field_name = $plugin['machine name'] . ':' . $field['machine name'];
  $field_stored_value = $stored_values[$plugin['machine name']][$field['machine name']]; ?>
  <p>
    <label for="<?php print $field_name; ?>"><?php print $name; ?></label>
    <input type="<?php print $field['type']; ?>" name="<?php print $field_name; ?>" value="<?php print isset($field_stored_value) ? $field_stored_value : ''; ?>" />
  </p>
<?php
}

function the_aggregated_feed() {
  global $social_aggregator;
  $posts = $social_aggregator->getPluginData(); ?>

  <ul class="social-aggregator-feed">
  <?php foreach($posts as $post) : ?>
    <li>
      <ul>
        <li><?php print $post['user_name']; ?></li>
        <li><?php print $post['post_content']; ?></li>
        <li><?php print $post['post_date']; ?></li>
      </ul>
    </li>
  <?php endforeach; ?>
  </ul>

<?php
}