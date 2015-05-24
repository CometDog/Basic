var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  var url = "http://api.openweathermap.org/data/2.5/weather?lat=" +
      pos.coords.latitude + "&lon=" + pos.coords.longitude;

  xhrRequest(url, 'GET', 
    function(responseText) {
      var json = JSON.parse(responseText);
      
      var country = json.sys.country;
      var cof;
      if (country == "US" || country == "BS" || country == "BZ" || country == "KY" || country == "PW" || country == "AR") {
        cof = 1;
      }
      else {
        cof = 0;
      }

      var temperature = Math.round(json.main.temp - 273.15);
      
      var conditions = json.weather[0].main;
      
      var dictionary = {
        "COUNTRY": cof,
        "TEMPERATURE": temperature,
        "CONDITIONS": conditions
      };

      MessageQueue.sendAppMessage(dictionary,
        function(e) {
          console.log("Weather info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending weather info to Pebble!");
        }
      );
    }      
  );
}

function locationError(err) {
  console.log("Error requesting location!");
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 5000, maximumAge: 60000}
  );
}

Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");

    getWeather();
  }
);