// Edit this file to add your customized JavaScript or load additional JavaScript files.


// Example 1 - Adding a language pack

// Here is an example of adding new language support, e.g. Polish:
// 1. Create a culture definition file for the new language (e.g. culture.pl.js) in the contrib folder, containing:
//      (function ($) {
//            $.globalization.availableCulture("pl", {
//                name: "pl",
//                englishName: "Polish",
//                nativeName: "polski",
//                stringBundle: "contrib/wrstrings.pl.js"
//            });
//        })(jQuery);
//  2. Copy ctxs.wrstrings.js from scripts/en/ctxs.wrstrings.js to contrib/wrstrings.pl.js
//  3. Edit wrstrings.pl.js and replace all the strings with translated ones. Save the file using UTF-8 encoding.
//  4. Add code to this file, custom.script.js, to load culture.pl.js:
//        $(document).ready(function () {
//            CTXS.Localization.getScript("contrib/culture.pl.js");
//        });


// Example 2: Adding a custom pre-login screen

// 1. Uncomment the following code, to display a pre-login screen showing a sample message and a "Continue..." link:
//        $(document).ready(function () {
//            var markup =
//                '<h3>Announcement</h3>' +
//                '<p>The cafeteria is closed next week - remember to bring your sandwiches!</p>' +
//                '<a href="#">Continue...</a>';
//
//            CTXS.Application.preLoginHook = function () {
//                // Add the markup to the prelogin-pane div, display the pane and attach a click handler to the <a> tag
//                $('#prelogin-pane').append($(markup)).ctxsDisplayPane().find('a').click(function () {
//                    // Publish the preLogin.done event to trigger the view to change
//                    CTXS.Events.publish(CTXS.Events.preLogin.done);
//                    return false;
//                });
//            };
//        });
// 2. Add the following CSS rules to custom.style.css:
// 
//        .custom-pane {
//	        text-align: center;
//	        color: white;
//        }
//        .custom-pane h3 {
//	        margin-top: 80px;
//	        font-size: 30px;
//        }
//        .custom-pane p {
//	        margin: 40px 0;
//	        font-size: 18px;
//        }
//        .custom-pane a, .custom-pane a:active, .custom-pane a:visited {
//	        font-size: 24px;
//	        color: tan;
//        }
//        .custom-pane a:hover {
//	        text-decoration: underline;
//        }
