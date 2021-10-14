module.exports = (sequelize, type) => {
  return sequelize.define('consumption', {
    id: {
      type: type.BIGINT,
      primaryKey: true,
      autoIncrement: true
    },
    name: {
      type: type.STRING,
      allowNull: false,
    },
    date: type.DATEONLY,
    value: {
      type: type.DECIMAL, defaultValue: 0
    },
  })
};
