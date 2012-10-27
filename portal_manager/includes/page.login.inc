<?php

/**
 * @file
 * Content for no access page
 */
function portal_noaccess_page() {
  
  $text = t('Please login.');
  $output = array(
      'first_para' => array(
          '#type' => 'markup',
          '#markup' => "<p>{$text}</p>",
      ),
  );
  return $output;

  
}