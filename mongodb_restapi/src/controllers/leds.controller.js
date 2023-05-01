const leds = require("../services/leds.service")

async function get(req, res, next) {
    try {
        res.json(await leds.getLedsStatus(req.params.chipId))
    } catch (err) {
        console.error(err)
        next(err)
    }
}

async function update(req, res, next) {
    try {
        res.json(
            await leds.updateLed(
                req.params.chipId,
                req.params.ledName,
                req.params.status
            )
        )
    } catch (err) {
        console.error(err)
        next(err)
    }
}

module.exports = {
    get,
    update,
}
