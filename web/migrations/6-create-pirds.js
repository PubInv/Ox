"use strict";
module.exports = {
  up: async (queryInterface, Sequelize) => {
    await queryInterface.createTable("pirds", {
      id: {
        allowNull: false,
        autoIncrement: true,
        //primaryKey: true,
        type: Sequelize.INTEGER,
      },
      event: {
        type: Sequelize.STRING,
      },
      type: {
        type: Sequelize.STRING,
      },
      loc: {
        type: Sequelize.STRING,
      },
      num: {
        type: Sequelize.INTEGER,
      },
      ms: {
        type: Sequelize.INTEGER,
      },
      val: {
        type: Sequelize.FLOAT,
      },
      sht: {
        type: Sequelize.INTEGER,
      },
      pid: {
        type: Sequelize.STRING,
      },
      time: {
        type: Sequelize.DATE,
      },
    });
  },
  down: async (queryInterface, Sequelize) => {
    await queryInterface.dropTable("pirds");
  },
};
