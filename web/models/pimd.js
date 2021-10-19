"use strict";
const { Model } = require("sequelize");
module.exports = (sequelize, DataTypes) => {
  class pimd extends Model {
    /**
     * Helper method for defining associations.
     * This method is not a part of Sequelize lifecycle.
     * The `models/index` file will call this method automatically.
     */
    static associate(models) {
      // define association here
    }
  }
  pimd.init(
    {
      location: DataTypes.STRING,
      value: DataTypes.INTEGER,
      time: DataTypes.DATE,
    },
    {
      sequelize,
      modelName: "pimd",
    }
  );
  return pimd;
};
