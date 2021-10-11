'use strict';
const {
  Model
} = require('sequelize');
module.exports = (sequelize, DataTypes) => {
  class page_loads extends Model {
    /**
     * Helper method for defining associations.
     * This method is not a part of Sequelize lifecycle.
     * The `models/index` file will call this method automatically.
     */
    static associate(models) {
      // define association here
    }
  };
  page_loads.init({
    userAgent: DataTypes.STRING,
    time: DataTypes.DATE
  }, {
    sequelize,
    modelName: 'page_loads',
  });
  return page_loads;
};