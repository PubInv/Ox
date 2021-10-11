'use strict';
module.exports = {
  up: async (queryInterface, Sequelize) => {
    await queryInterface.createTable('pimd', {
      id: {
        allowNull: false,
        autoIncrement: true,
        //primaryKey: true,
        type: Sequelize.INTEGER
      },
      location: {
        type: Sequelize.STRING
      },
      value: {
        type: Sequelize.INTEGER
      },
      time: {
        type: Sequelize.DATE
      },
      /*createdAt: {
        allowNull: false,
        type: Sequelize.DATE
      },
      updatedAt: {
        allowNull: false,
        type: Sequelize.DATE
      }*/
    });
  },
  down: async (queryInterface, Sequelize) => {
    await queryInterface.dropTable('pimd');
  }
};