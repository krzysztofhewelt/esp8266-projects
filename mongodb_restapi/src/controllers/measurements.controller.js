const measurements = require("../services/measurements.service")

async function getAll(req, res, next) {
    try {
        res.json(await measurements.getAll(req.params.chipId))
    } catch (err) {
        console.error(err)
        next(err)
    }
}

async function getBetweenDate(req, res, next) {
    try {
        res.json(
            await measurements.getBetweenDates(
                req.params.chipId,
                req.params.startDate,
                req.params.stopDate
            )
        )
    } catch (err) {
        console.error(err)
        next(err)
    }
}

async function create(req, res, next) {
    try {
        res.json(await measurements.addMeasurement(req.params.chipId, req.body))
    } catch (err) {
        console.error(err)
        next(err)
    }
}

module.exports = {
    getAll,
    getBetweenDate,
    create,
}
