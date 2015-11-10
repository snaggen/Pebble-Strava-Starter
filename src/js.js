var debug = true;

var locationOptions = {timeout: 15000, enableHighAccuracy: true, maximumAge: 0}; 
var accuracyMeters = 5;
var maxTry = 20;
var n = 0;

function fetch_location_data(pos) {
	if (debug) console.log('fetched_location ' + pos.coords.latitude + ',' + pos.coords.longitude + ' - ' + pos.coords.accuracy + 'm');
		if (pos.coords.accuracy <= accuracyMeters) {
			Pebble.sendAppMessage({GPS_COOR: Math.floor(pos.coords.latitude*10000+0.5)/10000 + ',' + Math.floor(pos.coords.longitude*10000+0.5)/10000});
		} else {
			if (n <= maxTry) {
				navigator.geolocation.getCurrentPosition(fetch_location_data, fetch_location_error, locationOptions);
				n++;
			}else Pebble.sendAppMessage({GPS_ERR:'Not accurate location'});
		}
}

function fetch_location_error(error) {
	if (debug) console.log("fetched_location_error");
	switch(error.code) {
		case error.PERMISSION_DENIED:
			Pebble.sendAppMessage({GPS_ERR:'GPS ERROR: PERMISSION_DENIED'});
			break;
		case error.POSITION_UNAVAILABLE:
			Pebble.sendAppMessage({GPS_ERR:'GPS ERROR: POSITION_UNAVAILABLE'});
			break;
		case error.TIMEOUT:
			Pebble.sendAppMessage({GPS_ERR:'GPS ERROR: TIMEOUT'});
			break;
		case error.UNKNOWN_ERROR:
			Pebble.sendAppMessage({GPS_ERR:'GPS ERROR: UNKNOWN_ERROR'});
			break;
    }
}

Pebble.addEventListener('ready', function(e) {
	if (navigator.geolocation) {
			navigator.geolocation.getCurrentPosition(fetch_location_data, fetch_location_error, locationOptions);
	} else {
		Pebble.sendAppMessage({GPS_ERR: "GPS ERROR: Geolocation is not supported"});
	}
});
