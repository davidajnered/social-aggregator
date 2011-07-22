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
add_action('admin_head', 'social_aggregator_head');
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
  $stored_values = get_option('social_aggregator_values');
  $field_name = $plugin['machine name'] . ':' . $field['machine name'];
  $field_stored_value = $stored_values[$plugin['machine name']][$field['machine name']]; ?>
  <p>
    <label for="<?php print $field_name; ?>"><?php print $name; ?></label>
    <input type="<?php print $field['type']; ?>" name="<?php print $field_name; ?>" value="<?php print social_aggregator_get_stored_value($field_name); ?>" />
  </p>
<?php
}

function social_aggregator_get_stored_value($field) {
  $values = get_option('social_aggregator_values');
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