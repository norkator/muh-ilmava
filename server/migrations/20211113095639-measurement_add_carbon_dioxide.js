'use strict';

module.exports = {
  up: async (queryInterface, Sequelize) => {
    /**
     * Add altering commands here.
     *
     * Example:
     * await queryInterface.createTable('users', { id: Sequelize.INTEGER });
     */
    await queryInterface.addColumn('measurements', 'incoming_air_carbon_dioxide', Sequelize.DECIMAL);
    await queryInterface.addColumn('measurements', 'incoming_air_carbon_monoxide', Sequelize.DECIMAL);
    await queryInterface.addColumn('measurements', 'outgoing_air_to_rooms_carbon_dioxide', Sequelize.DECIMAL);
    await queryInterface.addColumn('measurements', 'outgoing_air_to_rooms_carbon_monoxide', Sequelize.DECIMAL);
    await queryInterface.addColumn('measurements', 'returning_rooms_air_carbon_dioxide', Sequelize.DECIMAL);
    await queryInterface.addColumn('measurements', 'returning_rooms_air_carbon_monoxide', Sequelize.DECIMAL);
    await queryInterface.addColumn('measurements', 'waste_air_out_carbon_dioxide', Sequelize.DECIMAL);
    await queryInterface.addColumn('measurements', 'waste_air_out_carbon_monoxide', Sequelize.DECIMAL);
  },

  down: async (queryInterface, Sequelize) => {
    /**
     * Add reverting commands here.
     *
     * Example:
     * await queryInterface.dropTable('users');
     */
    await queryInterface.removeColumn('measurements', 'incoming_air_carbon_dioxide', Sequelize.DECIMAL);
    await queryInterface.removeColumn('measurements', 'incoming_air_carbon_monoxide', Sequelize.DECIMAL);
    await queryInterface.removeColumn('measurements', 'outgoing_air_to_rooms_carbon_dioxide', Sequelize.DECIMAL);
    await queryInterface.removeColumn('measurements', 'outgoing_air_to_rooms_carbon_monoxide', Sequelize.DECIMAL);
    await queryInterface.removeColumn('measurements', 'returning_rooms_air_carbon_dioxide', Sequelize.DECIMAL);
    await queryInterface.removeColumn('measurements', 'returning_rooms_air_carbon_monoxide', Sequelize.DECIMAL);
    await queryInterface.removeColumn('measurements', 'waste_air_out_carbon_dioxide', Sequelize.DECIMAL);
    await queryInterface.removeColumn('measurements', 'waste_air_out_carbon_monoxide', Sequelize.DECIMAL);
  }
};
