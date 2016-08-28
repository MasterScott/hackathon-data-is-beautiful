// NPM Modules
var express = require('express');
var geoip = require('geoip-lite');
var request = require('request');

var app = express();

const PORT = 8080;
const OPENPHISH_USERNAME = "";
const OPENPHISH_PASSWORD = "";

app.set('view engine', 'pug')

var phishes = {};
var phishesArr = [];
var minMaxArr = [];
var minVal = 0;
var maxVal = 0;

function getData(){
  console.log("Updating with new data");
  request.get("https://" + OPENPHISH_USERNAME + ":" + OPENPHISH_PASSWORD +"@openphish.com/prvt-ex/",
    function(err, resp, body){
      if(!err){
        var dataArr = body.split("\n");

        // The last point in the array is empty, so kill it.
        dataArr.splice(dataArr.length-1, 1);
        for(var i = 0; i < dataArr.length; i++){
          var ipData = geoip.lookup(JSON.parse(dataArr[i]).ip);
          var country = ipData.country;
          if(country === "US"){
              var region = ipData.region;
              if(phishes[region] === undefined){
                phishes[region] = {};
                phishes[region] = 1;
              } else {
                phishes[region] += 1;
              }
          }
          Object.keys(phishes).forEach(function(key){
            minMaxArr.push(phishes[key]);
            phishesArr.push([key, phishes[key]]);
          });
          minVal = Math.min.apply(0, minMaxArr);
          maxVal = Math.max.apply(0, minMaxArr);
        }
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
  return res.render('index', {data:phishesArr,minVal:minVal,maxVal:maxVal});
});

app.get('/about', function(req, res){
  return res.render('about');
});

app.listen(PORT, function(){
  console.log('Listening on port ' + PORT);
});
