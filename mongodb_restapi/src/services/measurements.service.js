const client = require("../services/db.service")

async function getAll(chipId) {
    return client
        .db(process.env.MONGODB_DATABASE)
        .collection("measurements")
        .find({ "metadata.chip_id": chipId })
        .toArray()
}

async function getBetweenDates(chipId, startDate, stopDate) {
    return client
        .db(process.env.MONGODB_DATABASE)
        .collection("measurements")
        .find({
            "metadata.chip_id": chipId,
            timestamp: {
                $gte: new Date(startDate),
                $lte: new Date(stopDate),
            },
        })
        .toArray()
}

async function addMeasurement(chipId, data) {
    return client
        .db(process.env.MONGODB_DATABASE)
        .collection("measurements")
        .insertOne({
            timestamp: new Date(),
            metadata: {
                IP: data.IP,
                chip_id: chipId,
                name: data.name,
            },
            humidity: data.humidity,
            temperature: data.temperature,
            wifi_rssi: data.wifi_rssi,
        })
}

module.exports = {
    getAll,
    getBetweenDates,
    addMeasurement,
}
