'use strict';

module.exports = {
  up: (queryInterface, Sequelize) => {
    return queryInterface.sequelize.query("CREATE EXTENSION IF NOT EXISTS timescaledb CASCADE;");
  },

  down: (queryInterface, Sequelize) => {
    return queryInterface.sequelize.query("DROP EXTENSION timescaledb;");
  }
};