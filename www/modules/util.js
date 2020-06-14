export function formatDuration(seconds) {
    var hours = Math.floor(seconds / 3600);
    var minutes = Math.round((seconds % 3600) / 60);
    var result = "";
    if(hours > 1) {
        result = hours + " hours";
    } else if (hours == 1) {
        result = hours + " hour";
    }
    if(minutes > 1) {
        result += " " + minutes + " minutes";
    } else if (minutes == 1) {
        result += " " + minutes + " minute";
    }
    return result;
}

export function hoursSinceToday() {
    var epoch = Math.round((new Date()).getTime() / 1000);
    return Math.ceil((epoch - epochMidNightToday()) / 3600);
}

export function epochMidNightToday() {
    var time = new Date();
    time.setHours(0, 0, 0, 0);
    return Math.round(time.getTime() / 1000);
}
