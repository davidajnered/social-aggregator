<?php
class SocialAggregatorVimeo extends SocialAggregatorPlugin {

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
    $fields[] = array(
      'title'   => 'Resources',
      'name'    => 'resource',
      'type'    => 'checkbox',
      'value'   => array(
        array(
          'name' => 'likes',
          'title' => 'Likes'
        ),
        array(
          'name' => 'videos',
          'title' => 'Videos',
        )
      ),
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
    $url = 'http://vimeo.com/api/v2/' . $params['username'] . '/likes.php';
    return unserialize($this->doCurl($url));
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