import {getCookie, setCookie} from './modules/cookie.js';
import {formatDuration, hoursSinceToday, epochMidNightToday} from './modules/util.js';

var excludedHours = 8;
var excludedHoursCookieKey = "excludedHours";
var colorMap = {
    1: 'rgba(214, 108, 96, 1)',
    2: 'rgba(204, 196, 88, 1)',
    3: 'rgba(85, 111, 189, 1)',
    4: 'rgba(87, 189, 97, 1)',
    5: 'rgba(200, 200, 200, 1)',
    // other
    0: 'rgba(100, 100, 100, 1)',
};

function loadSummary() {
    var data = {
        userID: 0,
        from: epochMidNightToday(),
    };

    $.getJSON("./get", data, function(json){
        // remaining un-documented time
        var otherSeconds = (hoursSinceToday() - excludedHours) * 3600;
        json.forEach(function(row) {
            otherSeconds -= row.TotalSeconds;
        })
        json.push({TypeID: 0, DisplayName: "Other", TotalSeconds: otherSeconds});

        // sort according to total length, DESC
        json.sort(function(a, b){return b.TotalSeconds - a.TotalSeconds;});

        // update chart
        var chartCtx = $('#summary-chart');
        var seconds = [];
        var labels = [];
        var colors = [];
        
        json.forEach(function(row) {
            seconds.push((row.TotalSeconds / 3600.0).toPrecision(2));
            labels.push(row.DisplayName);
            colors.push(colorMap[row.TypeID]);
        });
        var summaryChart = new Chart(chartCtx, {
            type: 'pie',
            data: {
                datasets: [{
                    data: seconds,
                    backgroundColor: colors,
                }],
                labels: labels,
            },
            options: {
                legend: {
                    position: "left",
                    labels: {
                        boxWidth: 20,
                    }
                },
            },
        });

        // update table
        var tbody = $('#summary-table')
        tbody.empty();
        json.forEach(function(row) {
            var tr = $('<tr/>');
            tr.append($('<td/>').text(row.DisplayName));
            tr.append($('<td/>').text(formatDuration(row.TotalSeconds)));
            tbody.append(tr);
        });
    }).fail(function(jqxhr, textStatus, error){
        window.alert("Failed to fetch summary: " + jqxhr.status + ". " + error);
    });
}

function updateExclude() {
    var hours = $('#excludedHours').val();
    excludedHours = parseInt(hours);
    if(isNaN(excludedHours)) {
        excludedHours = parseInt(getCookie(excludedHoursCookieKey) || "8");
        $('#excludedHours').addClass("is-invalid").val(excludedHours);
    } else {
        $('#excludedHours').removeClass("is-invalid");
    }
    setCookie(excludedHoursCookieKey, excludedHours, -1);
    loadSummary();
}

// init
$(function() {
    var excludeHoursCookie = getCookie(excludedHoursCookieKey);
    if(excludeHoursCookie != "") {
        excludedHours = parseInt(excludeHoursCookie);
    }
    $('#excludedHours').val(excludedHours);
    $('#excludeButton').click(updateExclude);
    loadSummary();
});
