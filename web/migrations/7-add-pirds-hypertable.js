"use strict";

module.exports = {
  up: async (queryInterface, Sequelize) => {
    await queryInterface.sequelize.query(
      "SELECT create_hypertable('pirds', 'time');"
    );
  },

  down: async (queryInterface, Sequelize) => {
  },
};
