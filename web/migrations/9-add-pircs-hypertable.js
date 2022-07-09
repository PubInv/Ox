"use strict";

module.exports = {
  up: async (queryInterface, Sequelize) => {
    await queryInterface.sequelize.query(
      "SELECT create_hypertable('pircs', 'time');"
    );
  },

  down: async (queryInterface, Sequelize) => {
  },
};
