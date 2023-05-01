const client = require("../services/db.service")

async function getLedsStatus(chipId) {
    return client
        .db(process.env.MONGODB_DATABASE)
        .collection("leds")
        .find({ chip_id: chipId })
        .toArray()
}

async function updateLed(chipId, ledName, status) {
    return client
        .db(process.env.MONGODB_DATABASE)
        .collection("leds")
        .findOneAndUpdate(
            { chip_id: chipId, led_name: ledName },
            { $set: { status: status, last_changed: new Date() } }
        )
}

module.exports = {
    getLedsStatus,
    updateLed,
}
