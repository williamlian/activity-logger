export function formatDuration(seconds) {
    var hours = Math.floor(seconds / 3600);
    var minutes = Math.round((seconds % 3600) / 60);
    var result = "";
    if(hours > 0) {
        result = hours + " 小时";
    }
    if(minutes >= 0) {
        result += " " + minutes + " 分钟";
    } 
    return result;
}

export function secondsSinceToday() {
    var epoch = Math.round((new Date()).getTime() / 1000);
    return epoch - epochMidNightToday();
}

export function epochMidNightToday() {
    var time = new Date();
    time.setHours(0, 0, 0, 0);
    return Math.round(time.getTime() / 1000);
}
