<script>

var device_serial;
{% if render_circuit_chart %}
{% for device in my_devices %}
    function get_circuit_chart{{ device.serial }}() {
        var circuits = [
            {% for circuit in device.circuits %}
            {{ circuit.pk }},
            {% endfor %}
        ];
        $('.circuit-pagination').bootpag({
            total: {% for circuit in device.circuits %}{% if forloop.last %}1{% else %}1+{% endif %}{% endfor %},
            hrefVariable: 1
        }).on("page", function(event, num){
            $.get('/charts/device/{{ device.serial }}/chart/',
                {
                    'stack': 'true',
                    'localtime': +(new Date()/1000),
                    'circuit_pk': circuits[num-1]
                }
            ).done(function(data) {
                $('#ajax-container2').html(data);
                render_circuitChart();
            });
        });
      device_serial = {{ device.serial }}
      $.get('/charts/device/{{ device.serial }}/chart/',
            {
                'stack': 'true',
                'localtime': +(new Date()/1000),
                'circuit_pk': circuits[0]
            }
        ).done(function(data) {
            $('#ajax-container2').html(data);
            render_circuitChart();
        });
    }
{% endfor %}

$(function() {
    $.get('/charts/device/{{ my_devices.0.serial }}/chart/',
        {
            'stack': 'true',
            'localtime': +(new Date()/1000),
        },
        function(data) {
            $('#ajax-container1').html(data);
        }
    );
    get_circuit_chart{{ my_devices.0.serial}}();
});
{% endif %}

function refresh_chart(stack) {
  var device_name = $('#chart-title').html().split(' at a glance')[0]
  $.get('/charts/device/'+device_serial+'/chart/',
    {
      'stack': stack,
      'localtime': +(new Date()/1000),
    },
    function(data) {
      $('#ajax-container1').html(data);
      //TODO: blocks other ajax. Move to worker thread?
      //render_chart();
      $('#chart-title').html(device_name+" at a glance");
      if (stack) {
        $('#chart-type-label').html('Stacked <span class="caret"></span>');
      } else {
        $('#chart-type-label').html('Unstacked <span class="caret"></span>');
      }
    }
  );
}

var contentArray = {}
{% for device in my_devices %}
contentArray['{{ device.serial }}'] = '';
var marker{{ device.serial }};
var infowindow{{ device.serial }} = new google.maps.InfoWindow();
var dataDimensions{{ device.serial }} = [0,0]
{% endfor %}

function get_wattage_usage() {
   {% for device in my_devices %}
    $.get('/device/get_wattage_usage/',
        { serial: {{ device.serial }} },
        function(data) {
           var current_wattage, average_wattage, cost_today;
           if (data.average_wattage !== null) {
               if (data.average_wattage > data.current_wattage) {
                   current_wattage = '<p><strong>Current Usage: </strong><span style="color: green;" id="current-wattage-{{ device.serial }}">'+data.current_wattage+'</span></p>'
               } else {
                   current_wattage = '<p><strong>Current Usage: </strong><span style="color: red;" id="current-wattage-{{ device.serial }}">'+data.current_wattage+'</span></p>'
               }
               average_wattage = '<p><strong>Average Usage:</strong><span id=average-wattage-{{ device.serial }}>'+data.average_wattage+'</span></p>'
               cost_today = '<p><strong>Cost Today: </strong><span id=cost-today-{{ device.serial }}>$'+data.cost_today+'</span></p>';
            }
            contentArray['{{ device.serial }}'] = '<div id="content"><h3 id="device-name">{{ device.name }}</h3><div id="body-content-{{ device.serial }}">'+ average_wattage +'<p>'+cost_today+'</p>'+ current_wattage +'</div></div>';
           infowindow{{ device.serial }}.setContent(contentArray['{{ device.serial }}']);
           {% if forloop.first %}
           infowindow{{ device.serial }}.open(map, marker{{ device.serial }});
           {% endif %}

           google.maps.event.addListener(marker{{ device.serial }}, 'click', function() {
             change_dashboard_device{{ device.serial }}();
           });
        }
     );
   {% endfor %}
}

{% for device in my_devices %}
    var updateDashboardTimeout{{ device.serial }};
    // Function to update device-specific elements on the dashboard for device with serial {{ device.serial }}
    function change_dashboard_device{{ device.serial }}(infowindow, marker) {
        console.log("Device change");
        {% for device in my_devices %}
        infowindow{{ device.serial }}.close();
        try {
          clearTimeout(updateDashboardTimeout{{ device.serial }});
        } catch (e) { console.log(e); }
        {% endfor %}
        infowindow{{ device.serial }}.open(map, marker{{ device.serial }});
        {% if render_circuit_chart %}
        get_circuit_chart{{ device.serial }}();
        {% endif %}
        $.get('/charts/device/{{ device.serial }}/chart/',
            {
                'stack': true,
                'localtime': +(new Date)/1000,
            },
            function(data) {
                $('#ajax-container1').html(data);
                //render_chart();
                $('#chart-title').html("{{ device.name }} at a glance");
                $('#chart-title').val("{{ device.serial }}");
                device_serial = {{ device.serial }};
            }
        );
         $.get('/charts/billing_information/',
            {
               'serial': {{ device.serial }}
            },
            function(data) {
               $('#ajax-billing-information').html(data);
            }
         );

         $.get('/charts/circuits_information/',
            {
               'serial': {{ device.serial }}
            },
            function(data) {
               if (device_serial == {{ device.serial }})
                 $('#ajax-circuits-information').html(data);
            }
         );
    }
{% endfor %}

var map_open = true;
var realtime_status = true;

var last_timestamp;

var chart, circuitChart;

function getChartData(from, to, serial) {
  try {
   var displayUnit = chart.displayUnit().slice(-1);
   var start = from;
   var stop = to;
   var get_url = '/charts/device/'+serial+'/data/'
   $.get(get_url,
      {
         'unit':displayUnit,
         'from':start,
         'to':stop,
         'localtime': +(new Date)/1000,
      },
      function (ajax_data) {
         if (ajax_data.data[0][0] != null) {
            chart.addData({unit: displayUnit, values: ajax_data.data});
         }
      }
   );
  } catch (e) {
    console.info("Chart not defined yet");
 }
}

function toggle_chart_realtime() {
  if (realtime_status) {
       $('#span-realtime').removeClass('glyphicon-pause');
       $('#span-realtime').addClass('glyphicon-play');
       chart.settings.area.followAnchor = false;
       last_timestamp = +(new Date())/1000;
    } else {
       $('#span-realtime').removeClass('glyphicon-play');
       $('#span-realtime').addClass('glyphicon-pause');
       chart.settings.area.followAnchor = true;
       getChartData(last_timestamp, +(new Date()/1000), device_serial);
    }
    realtime_status = !realtime_status;
    console.log("realtime_status: "+realtime_status)
}

$(function() {
   get_wattage_usage();
   initialize();
   $('#map-slide-toggle').on('click', function() {
      if (map_open) {
         $('#map-canvas').slideUp();
         $('#map-slide-text').html('Show Map');
         $('#map-slide-icon').removeClass('glyphicon-menu-up');
         $('#map-slide-icon').addClass('glyphicon-menu-down')
      } else {
         $('#map-canvas').slideDown();
         $('#map-slide-text').html('Hide Map');
         $('#map-slide-icon').removeClass('glyphicon-menu-down');
         $('#map-slide-icon').addClass('glyphicon-menu-up');
      }
      map_open = !map_open;
   });
   
   $('#btn-realtime').on('click', function() {
      toggle_chart_realtime();
   });

   $.get('/charts/billing_information/',
       {
         'serial': {{ my_devices.0.serial }}
       },
       function(data) {
         $('#ajax-billing-information').html(data);
       }
   );
   
   $.get('/charts/circuits_information/',
       {
         'serial': {{ my_devices.0.serial }}
       },
       function(data) {
         $('#ajax-circuits-information').html(data);
       }
   );

});

var map, placeSearch, autocomplete;

function initialize() {
  var mapOptions = {
    scrollwheel: false,
    zoom: 17
  };
  map = new google.maps.Map(document.getElementById('map-canvas'),
      mapOptions);
      
   var LatLngList = new Array ();

  {% for device in my_devices %}
     var pos = new google.maps.LatLng({{ device.position }});
     LatLngList.push(pos);

     marker{{ device.serial }} = new google.maps.Marker({
       map: map,
       position: pos,
       title: '{{ device.name }}',
       label: '{{ device.name }}',
       // http://stackoverflow.com/questions/7095574/google-maps-api-3-custom-marker-color-for-default-dot-marker
       {% if device.owner != user %}
       icon: 'http://chart.apis.google.com/chart?chst=d_map_pin_letter&chld=%E2%80%A2|F38630'
       {% else %}
       icon: 'http://chart.apis.google.com/chart?chst=d_map_pin_letter&chld=%E2%80%A2|FF6B6B'
       {% endif %}
     });

     

  {% endfor %}
  
   new DayNightOverlay({
      map: map
   });

   var bounds = new google.maps.LatLngBounds ();
   //  Go through each...
   for (var i = 0, LtLgLen = LatLngList.length; i < LtLgLen; i++) {
      //  And increase the bounds to take this point
      bounds.extend (LatLngList[i]);
   }
   //  Fit these bounds to the map
   map.fitBounds (bounds);
}
</script>