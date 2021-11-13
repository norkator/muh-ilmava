module.exports = (sequelize, type) => {
  return sequelize.define('measurement', {
    id: {
      type: type.BIGINT,
      primaryKey: true,
      autoIncrement: true
    },

    incoming_air_temperature: type.DECIMAL,
    incoming_air_humidity: type.DECIMAL,
    incoming_air_carbon_dioxide: type.DECIMAL,
    incoming_air_carbon_monoxide: type.DECIMAL,

    outgoing_air_to_rooms_temperature: type.DECIMAL,
    outgoing_air_to_rooms_humidity: type.DECIMAL,
    outgoing_air_to_rooms_carbon_dioxide: type.DECIMAL,
    outgoing_air_to_rooms_carbon_monoxide: type.DECIMAL,

    returning_rooms_air_temperature: type.DECIMAL,
    returning_rooms_air_humidity: type.DECIMAL,
    returning_rooms_air_carbon_dioxide: type.DECIMAL,
    returning_rooms_air_carbon_monoxide: type.DECIMAL,

    waste_air_out_temperature: type.DECIMAL,
    waste_air_out_humidity: type.DECIMAL,
    waste_air_out_carbon_dioxide: type.DECIMAL,
    waste_air_out_carbon_monoxide: type.DECIMAL,


  })
};
