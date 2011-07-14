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

$social_aggregator;

add_action('init', 'social_aggregator_init');
function social_aggregator_init() {
  require_once('SocialAggregator.inc');
  require_once('SocialAggregatorPlugin.inc');
  require_once('SocialAggregatorPlugin.interface');
  global $social_aggregator;
  $social_aggregator = new SocialAggregator();
}

register_activation_hook(__FILE__,'social_aggregator_install');
function social_aggregator_install() {
  global $wpdb;
  $table_name = $wpdb->prefix . "social_aggregator";

  $sql = "CREATE TABLE " . $table_name . " (
    id varchar(64) NOT NULL,
    plugin text NOT NULL,
    date int(11) NOT NULL,
    content longtext NOT NULL,
    PRIMARY KEY (id)
  ) ENGINE=InnoDB DEFAULT CHARSET=latin1";

  require_once(ABSPATH . 'wp-admin/includes/upgrade.php');
  dbDelta($sql);
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
  global $social_aggregator;
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
    foreach($_POST as $field => $value) {
      if(!in_array($field, $ignore)) {
        // Explode name to get [0] = plugin machine name, [1] = field name
        // We only use this for plugin fields where we have to know what plugin
        // can we use social_aggregator_get_field_value here instead?
        $parts = explode(':', $field);
        if(isset($parts[1])) {
          $stored_values[$parts[0]][$parts[1]] = $value;
        }
        else {
          $stored_values[$field] = $value;
        }
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
    <?php $social_aggregator->adminForm(); ?>
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
    <!--<input type="<?php print $field['type']; ?>" name="<?php print $field_name; ?>" value="<?php print isset($field_stored_value) ? $field_stored_value : ''; ?>" />-->
    <input type="<?php print $field['type']; ?>" name="<?php print $field_name; ?>" value="<?php print social_aggregator_get_stored_value($field_name); ?>" />
  </p>
<?php
}

function social_aggregator_get_stored_value($field) {
  $values = get_option('social_aggregator_stored_values');
  $parts = explode(':', $field);
  if(isset($parts[1])) {
    return $values[$parts[0]][$parts[1]];
  }
  return $values[$field];
}

function the_aggregated_feed() {
  global $social_aggregator;
  $posts = $social_aggregator->data();
}

/*
<!--<input type="hidden" name="<?php echo $hidden_submit_field; ?>" value="Y">
<h3>Plugins:</h3>
<?php foreach($plugins as $plugin) : ?>
  <fieldset>
    <legend><?php print $plugin['plugin name']; ?></legend>
    <?php foreach($plugin['user config'] as $name => $field) {
      social_aggregator_get_formated_field($plugin, $name, $field);
    } ?>
  </fieldset>
<?php endforeach; ?>

<fieldset>
  <h3>Global Settings</h3>
  <p>
    <label for="">Update Intervals<label>
    <input type="text" name="update_intervals" value="<?php print social_aggregator_get_stored_value('update_intervals'); ?>" />
  </p>
  <p>
    <label for="">Request Timeout<label>
    <input type="text" name="request_timeout" value="<?php print social_aggregator_get_stored_value('request_timeout'); ?>" />
  </p>
</fieldset>

<p class="submit">
  <input type="submit" name="Submit" class="button-primary" value="Save Changes" />
</p>-->
*/