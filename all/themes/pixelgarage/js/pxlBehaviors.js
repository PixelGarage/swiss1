/**
 * This file contains all Drupal behaviours of the Apia theme.
 *
 * Created by ralph on 05.01.14.
 */

(function ($) {

  /**
   * Set a class defining the device, e.g. mobile-device or desktop.
   */
  Drupal.behaviors.setMobileClass = {
    attach: function (context) {
      if (isMobile.any) {
        $('body').addClass('mobile-device');
      }
      else {
        $('body').addClass('desktop');
      }
    }
  };

  /**
   * This behavior adds shadow to header on scroll.
   *
   */
  Drupal.behaviors.addHeaderShadow = {
    attach: function (context) {
      $(window).on("scroll", function () {
        if ($(window).scrollTop() > 10) {
          $("header.navbar .container").css("box-shadow", "0 4px 3px -4px gray");
        }
        else {
          $("header.navbar .container").css("box-shadow", "none");
        }
      });
    }
  };

  /**
   * Defines behavior of a video stream in full view mode.
   */
  Drupal.behaviors.VideoStreamBehavior = {
    attach: function (context) {
      var $node = $('.modal .node-videostream'),
          $videoContainer = $node.find('.video-container');

      // if video exists, catch click events
      $videoContainer.once('click', function() {
        $(this).on('click', function() {
          //
          // hide all elements of node, if the video is playing
          // otherwise show all node elements
          var $video = $videoContainer.find('video'),
              video = $video[0],
              $header = $node.find('.video-header'),
              $body = $node.find('.video-body'),
              $similar = $node.find('.video-similar');

          // show video controls when video ends
          $video.once('events', function() {
            $(this).on('play', function () {
              // video is playing, hide all node elements
              $header.addClass('video-playing');
              $body.addClass('video-playing');
              $similar.addClass('video-playing');
            });
            $(this).on('pause ended', function () {
              // video is paused or ended, hide all node elements
              $header.removeClass('video-playing');
              $body.removeClass('video-playing');
              $similar.removeClass('video-playing');
            });
          });

        });
      });
    }
  };

  /**
   * Swaps images from desaturated to colored on mouse hover.
   */
  Drupal.behaviors.hoverImageSwap = {
    attach: function () {
      var $item = $('.pe-container > .pe-item');

      $item.hover(
        function () {
          // mouse enter
          var $thumb = $(this).find('.node-videostream .video-thumb');

          if ($thumb.length > 0) {
            var thumb_uri = $thumb.attr('src');
            $thumb.attr('src', thumb_uri.replace('files/styles/desaturate_25/public/images', 'files/images'));
          }
        },
        function () {
          // mouse leave
          var $thumb = $(this).find('.node-videostream .video-thumb');

          if ($thumb.length > 0) {
            var thumb_uri = $thumb.attr('src');
            $thumb.attr('src', thumb_uri.replace('files/images', 'files/styles/desaturate_25/public/images'));
          }
        }
      );
    }
  };

  /**
   * Handles the next / previous buttons in exposed form of program view.
   */
  Drupal.behaviors.viewsProgramExposedButtons = {
    attach: function (context) {
      var $exposedForm = $('#views-exposed-form-program-block-program'),
          $previous = $exposedForm.find('.views-previous-day-button'),
          $next = $exposedForm.find('.views-next-day-button'),
          $select = $exposedForm.find('#edit-field-broadcast-date-select');

      $previous.once('click', function() {
        $(this).on('click', function() {
          var $select_option = $select.find('>option:selected'),
            $prev_option = $select_option.prev('option');

          if ($prev_option.length > 0) {
            var prev_val = $prev_option.val();

            $select_option.removeAttr('selected');
            $prev_option.attr('selected', 'selected');
            $select.val(prev_val);
            $select.change();
          }
        });
      });

      $next.once('click', function() {
        $(this).on('click', function() {
          var $select_option = $select.find('>option:selected'),
              $next_option = $select_option.next('option');

          if ($next_option.length > 0) {
            var next_val = $next_option.val();

            $select_option.removeAttr('selected');
            $next_option.attr('selected', 'selected');
            $select.val(next_val);
            $select.change();
          }
        });
      });

    }
  };

  Drupal.behaviors.animExposedSearchForm = {
    attach: function(context) {
      var $searchContainer = $('#navbar-search-container'),
          $searchForm = $searchContainer.find('.navbar-search-form'),
          $searchButton = $searchContainer.find('.navbar-search-button'),
          $submitButton = $searchForm.find('#edit-submit-search-videos');

      $searchButton.once('click', function() {
        $(this).on('click', function() {
          if ($(window).width() < 640) {
            // activate search button as modal toggle
            $searchButton.attr('data-toggle', 'modal');
          }
          else {
            // de-activate search button as modal toggle
            $searchButton.removeAttr('data-toggle');

            // expand/submit behavior
            if ($searchForm.hasClass('form-expanded')) {
              // submit search
              //$searchForm.removeClass('form-expanded');
              $submitButton.click();
            }
            else if ($(window).width() >= 640) {
              // expand search form, if bigger than tablet size
              $searchForm.addClass('form-expanded');
            }
          }
        });

        // reset form-expanded on resize
        $(window).on('resize', function() {
          $searchForm.removeClass('form-expanded');
        });
      });
    }
  };

  /**
   * Anchor menus: Scrolls smoothly to anchor due to menu click.
  Drupal.behaviors.smoothScrolltoAnchors = {
    attach: function(context, settings) {
      $(function() {
        $('.menu li.leaf a').click(function() {
          var anchorPos = this.href.indexOf('#');
          // no anchor available, perform click
          if (anchorPos == -1) return true;

          // menu item references anchor, get anchor target
          var target = $(this.href.slice(pos));
          if (target.length) {
            $('html, body').stop().animate({
              scrollTop: target.offset().top
            }, 1000, 'swing');
            return false;
          }
          // no target available, perform click
          return true;
        });
      });
    }
  };
   */

  /**
   * Allows full size clickable items.
   Drupal.behaviors.fullSizeClickableItems = {
    attach: function () {
      var $clickableItems = $('.node-link-item.node-teaser .field-group-div')
        .add('.node-team-member.node-teaser .field-group-div');

      $clickableItems.once('click', function () {
        $(this).on('click', function () {
          window.location = $(this).find("a:first").attr("href");
          return false;
        });
      });
    }
  };
   */

  /**
   * Swaps images from black/white to colored on mouse hover.
   Drupal.behaviors.hoverImageSwap = {
    attach: function () {
      $('.node-project.node-teaser .field-name-field-images a img').hover(
        function () {
          // mouse enter
          src = $(this).attr('src');
          $(this).attr('src', src.replace('teaser_bw', 'teaser_normal'));
        },
        function () {
          // mouse leave
          src = $(this).attr('src');
          $(this).attr('src', src.replace('teaser_normal', 'teaser_bw'));
        }
      );
    }
  }
   */

  /**
   * Open file links in its own tab. The file field doesn't implement this behaviour right away.
   Drupal.behaviors.openDocumentsInTab = {
    attach: function () {
      $(".field-name-field-documents").find(".field-item a").attr('target', '_blank');
    }
  }
   */

})(jQuery);
