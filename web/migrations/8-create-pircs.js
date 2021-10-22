"use strict";
module.exports = {
  up: async (queryInterface, Sequelize) => {
    await queryInterface.createTable("pircs", {
      id: {
        allowNull: false,
        autoIncrement: true,
        //primaryKey: true,
        type: Sequelize.INTEGER,
      },
      ack: {
        type: Sequelize.STRING,
      },
      err: {
        type: Sequelize.INTEGER,
      },
      com: {
        type: Sequelize.STRING,
      },
      par: {
        type: Sequelize.STRING,
      },
      int: {
        type: Sequelize.STRING,
      },
      mod: {
        type: Sequelize.INTEGER,
      },
      val: {
        type: Sequelize.INTEGER,
      },
      time: {
        type: Sequelize.DATE,
      },
    });
  },
  down: async (queryInterface, Sequelize) => {
    await queryInterface.dropTable("pircs");
  },
};
