"use strict";
const { Model } = require("sequelize");
module.exports = (sequelize, DataTypes) => {
  class pirds extends Model {
    /**
     * Helper method for defining associations.
     * This method is not a part of Sequelize lifecycle.
     * The `models/index` file will call this method automatically.
     */
    static associate(models) {
      // define association here
    }
  }
  pirds.init(
    {
      event: DataTypes.STRING,
      type: DataTypes.STRING,
      loc: DataTypes.STRING,
      num: DataTypes.INTEGER,
      ms: DataTypes.INTEGER,
      val: DataTypes.FLOAT,
      sht: DataTypes.INTEGER,
      pid: DataTypes.STRING,
      time: DataTypes.DATE,
    },
    {
      sequelize,
      modelName: "pirds",
    }
  );
  return pirds;
};
