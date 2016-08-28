// NPM Modules
var express = require('express');
var geoip = require('geoip-lite');
var request = require('request');

var app = express();

const PORT = 8080;

app.set('view engine', 'pug')

var rawCountryCodes = {};
var countryCodes = [];

function getData(){
  console.log("Updating with new data");
  request.get("https://username:password@openphish.com/prvt-ex/",
    function(err, resp, body){
      if(!err){
        var dataArr = body.split("\n");

        // The last point in the array is empty, so kill it.
        dataArr.splice(dataArr.length-1, 1);
        for(var i = 0; i < dataArr.length; i++){
          var country = geoip.lookup(JSON.parse(dataArr[i]).ip).country;
          // There might be a better way to do this, but we are making an object and then re-creating that object into arrays
          if(rawCountryCodes[country] === undefined){
            rawCountryCodes[country] = 1;
          } else {
            rawCountryCodes[country] += 1;
          }
        }
        Object.keys(rawCountryCodes).forEach(function(key){
          countryCodes.push([key, rawCountryCodes[key]]);
        });
        console.log("Finished updating.")
      } else {
        console.log(err)
      }
    });

}
getData();
setInterval(getData, 600000);

app.use('/public', express.static("bower_components"));

app.get('/', function(req, res){
  return res.render('index', {data:countryCodes});
});

app.get('/api', function(req, res){
  return res.json({'data':countryCodes});
});

app.get('/about', function(req, res){
  return res.render('about');
});

app.listen(PORT, function(){
  console.log('Listening on port ' + PORT);
});
