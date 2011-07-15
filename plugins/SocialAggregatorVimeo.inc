<?php
class SocialAggregatorVimeo implements SocialAggregatorPluginInterface {

  public $pluginName = 'Vimeo';

  /**
   * User details
   */
  public function config() {
    $fields[] = array(
      'title' => 'Username',
      'name' => 'username',
      'type' => 'text',
    );
    $fields['resources'][] = array(
      'title'   => 'Likes',
      'name'    => 'resource[]',
      'value'   => 'likes',
      'type'    => 'checkbox',
    );
    $fields['resources'][] = array(
      'title'   => 'Videos',
      'name'    => 'resource[]',
      'value'   => 'videos',
      'type'    => 'checkbox',
    );
    return $fields;
  }

  /**
   * Implementation of hook_data
   * params[0] is an array containing all the stored data
   * Get the data from service api
   * most be returns in an structured array
   */
  public function data($params) {
    $ch = curl_init('http://vimeo.com/api/v2/davidajnered/likes.php');
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_POST, 0);
    $result = curl_exec($ch);
    curl_close($ch);
    return unserialize($result);
  }

  /**
   * Return the date when the post was created
   */
  public function timestamp($data) {
    return strtotime($data['liked_on']);
  }

  /**
   * Print data to view
   */
  public function render($data) { ?>

    <div>
      <h2>Vimeo: <?php print $data['title']; ?></h2>
      <img src="<?php print $data['thumbnail_medium']; ?>"><br/>
      <?php print substr($data['description'], 0, 100); ?>...
    </div>

  <?php
  }
}