"use strict";
const { Model } = require("sequelize");
module.exports = (sequelize, DataTypes) => {
  class pircs extends Model {
    /**
     * Helper method for defining associations.
     * This method is not a part of Sequelize lifecycle.
     * The `models/index` file will call this method automatically.
     */
    static associate(models) {
      // define association here
    }
  }
  pircs.init(
    {
      ack: DataTypes.STRING,
      err: DataTypes.INTEGER,
      com: DataTypes.STRING,
      par: DataTypes.STRING,
      int: DataTypes.STRING,
      mod: DataTypes.INTEGER,
      val: DataTypes.INTEGER,
      time: DataTypes.DATE,
    },
    {
      sequelize,
      modelName: "pircs",
    }
  );
  return pircs;
};
