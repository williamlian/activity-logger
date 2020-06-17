import {getCookie, setCookie} from './modules/cookie.js';
import {formatDuration, secondsSinceToday, epochMidNightToday} from './modules/util.js';

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
var blinkSpeed = 100;
var pieChart = null;
var activeType = -1;
var userID = 0;

function loadSummary() {
    var data = {
        userID: userID,
        from: epochMidNightToday(),
    };

    $.getJSON("./get", data, function(json){
        // remaining un-documented time
        var otherSeconds = secondsSinceToday() - excludedHours * 3600;
        json.forEach(function(row) {
            otherSeconds -= row.TotalSeconds;
        });
        
        // sort according to total length, DESC
        json.sort(function(a, b){return b.TotalSeconds - a.TotalSeconds;});

        if(otherSeconds > 0) {
            // always add other at the end
            json.push({TypeID: 0, DisplayName: "其他", TotalSeconds: otherSeconds});
        }

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
        if(pieChart == null) {
            pieChart = new Chart(chartCtx, {
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
            })
        } else {
            pieChart.data = {
                datasets: [{
                    data: seconds,
                    backgroundColor: colors,
                }],
                labels: labels,
            };
            pieChart.update(0);
        }

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

function loadCurrent() {
    var data = {
        userID: userID,
        type: "last",
    };
    $.getJSON("./get", data, function(json){
        var text;
        var bgcolor;
        var color;
        if(json.EndedAt == null) {
            text = "当前活动: " + json.Type.DisplayName;
            bgcolor = colorMap[json.Type.ID];
            color = 'black';
            activeType = json.Type.ID;
        } else {
            text = "当前没有任何活动";
            bgcolor = colorMap[0];
            color = 'white';
            activeType = -1;
        }
        $('#current-activity').css({backgroundColor: bgcolor, color: color}).text(text);
        activateFooterType(activeType, false);
    }).fail(function(jqxhr, textStatus, error){
        window.alert("Failed to fetch summary: " + jqxhr.status + ". " + error);
    });
}

function loadTypes() {
    $.getJSON("./types", {}, function(json){
        var footer = $('#activity-footer');
        json.forEach(function(t) {
            var btn = $('<button class="btn btn-sm"/>');
            btn.css({
                borderColor: colorMap[t.ID], 
                borderWidth: '1px',
                borderStyle: 'solid',
            });
            btn.text(t.DisplayName);
            btn.data("activity-type-id", t.ID);
            btn.click(() => changeType(t.ID));
            footer.append(btn);
        });
    }).fail(function(jqxhr, textStatus, error){
        window.alert("Failed to fetch types: " + jqxhr.status + ". " + error);
    });
}

var blinkIntervals = {};
function activateFooterType(typeID, blink) {
    var buttons = $('#activity-footer button');
    buttons.each(function() {
        var btn = $(this);
        var btnID = btn.data("activity-type-id");
        if(btnID == typeID) {
            // a button should be activated / blinked
            if(blink) {
                blinkIntervals[btnID] = setInterval(getBlinker(this), blinkSpeed);
            } else {
                if(blinkIntervals[btnID]) {
                    clearInterval(blinkIntervals[btnID]);
                    delete blinkIntervals[btnID];
                }
                btn.css({backgroundColor: colorMap[btnID]});
            }
        } else {
            // button should not be activated
            if(blinkIntervals[btnID]) {
                clearInterval(blinkIntervals[btnID]);
                delete blinkIntervals[btnID];
            }
            btn.css({backgroundColor: 'transparent'});
        }
    });
}

function getBlinker(e) {
    return function() {
        var btn = $(e);
        if(btn.data('blinkOn')) {
            btn.css('backgroundColor', 'transparent').data('blinkOn', false);
        } else {
            btn.css('backgroundColor', colorMap[btn.data("activity-type-id")]).data('blinkOn', true);
        }
    };
}

function changeType(typeID) {
    activateFooterType(typeID, true);
    var path = "./start";
    if(activeType == typeID) {
        path = "./end";
    }
    var data = {
        UserID: userID,
        TypeID: typeID,
    };
    $.post(path, JSON.stringify(data), function(json){
        loadCurrent();
    }).fail(function(jqxhr, textStatus, error){
        window.alert("Failed to start/end: " + jqxhr.status + ". " + error);
    }, 'json');
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
    loadTypes();
    loadSummary();
    loadCurrent();
    setInterval(loadCurrent, 5000);
    setInterval(loadSummary, 5000);
});
