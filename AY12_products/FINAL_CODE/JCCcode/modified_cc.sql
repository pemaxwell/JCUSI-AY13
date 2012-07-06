-- MySQL Administrator dump 1.4
--
-- ------------------------------------------------------
-- Server version	5.5.9


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;


--
-- Create schema commandcenter_db
--

CREATE DATABASE IF NOT EXISTS commandcenter_db;
USE commandcenter_db;


--
-- Definition of table `missionlookup`
--

DROP TABLE IF EXISTS `missionlookup`;
CREATE TABLE `missionlookup` (
  `missionID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `missionType` varchar(45) NOT NULL,
  `missionName` varchar(45) NOT NULL,
  PRIMARY KEY (`missionID`)
) ENGINE=InnoDB AUTO_INCREMENT=14 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `missionlookup`
--

/*!40000 ALTER TABLE `missionlookup` DISABLE KEYS */;
INSERT INTO `missionlookup` (`missionType`,`missionName`) VALUES 
 (1,'Scanning'),
 (13,'Moving');
/*!40000 ALTER TABLE `missionlookup` ENABLE KEYS */;


--
-- Definition of table `robots`
--

DROP TABLE IF EXISTS `robots`;
CREATE TABLE `robots` (
  `robotID` varchar(10) NOT NULL,
--  `currentMission` int(10) unsigned NOT NULL,
  `robotType` int(10) unsigned NOT NULL,
  `ipAddress` varchar(20) NOT NULL,
  PRIMARY KEY (`robotID`),
  KEY `FK_robots_1` (`robotType`),
--  KEY `FK_robots_2` (`currentMission`),
  CONSTRAINT `FK_robots_1` FOREIGN KEY (`robotType`) REFERENCES `robottypelookup` (`typeID`)
--  CONSTRAINT `FK_robots_2` FOREIGN KEY (`currentMission`) REFERENCES `missionlookup` (`missionID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `robots`
--

/*!40000 ALTER TABLE `robots` DISABLE KEYS */;
INSERT INTO `robots` (`robotID`,`robotType`,`ipAddress`) VALUES 
 ('22240',1,'192.168.1.102:1234');
/*!40000 ALTER TABLE `robots` ENABLE KEYS */;


--
-- Definition of table `robotstate`
--

DROP TABLE IF EXISTS `robotstate`;
CREATE TABLE `robotstate` (
  `stateID` int NOT NULL AUTO_INCREMENT,
  `robotID` varchar(10) NOT NULL,
  `timeStamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `packetType` varchar(10) NOT NULL,
  `timeDrive` int(10),
  `latitude` double NOT NULL,
  `longitude` double NOT NULL,
  `elevation` float NOT NULL,
  `speed` float NOT NULL,
  `operatingLife` float NOT NULL,
  `robotBearing` double NOT NULL,
  `sensorBearing` double NOT NULL,
  `currentMission` int unsigned NOT NULL,
  
  PRIMARY KEY (`stateID`),
  KEY `FK_location_1` (`robotID`),
  KEY `FK_location_2` (`currentMission`),
  CONSTRAINT `FK_robotstate_1` FOREIGN KEY (`robotID`) REFERENCES `robots` (`robotID`),
  CONSTRAINT `FK_robotstate_2` FOREIGN KEY (`currentMission`) REFERENCES `missionlookup` (`missionID`)
) ENGINE=InnoDB AUTO_INCREMENT=1153 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `robotstate`
--

/*!40000 ALTER TABLE `robotstate` DISABLE KEYS */;
INSERT INTO `robotstate` (`robotID`,`timeStamp`, `packetType`,`timeDrive`,`latitude`,`longitude`,`elevation`,`speed`,`operatingLife`,`robotBearing`,`sensorBearing`, `currentMission`)
VALUES ('22240','2011-04-28 15:51:07','robotState',1875,41.3905233333,-73.9532633333,15.4,27,1.5, 240.12331, 250.234, 13);

/*!40000 ALTER TABLE `robotstate` ENABLE KEYS */;


--
-- Definition of table `robottypelookup`
--

DROP TABLE IF EXISTS `robottypelookup`;
CREATE TABLE `robottypelookup` (
  `typeID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `type` varchar(30) NOT NULL,
  PRIMARY KEY (`typeID`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `robottypelookup`
--

/*!40000 ALTER TABLE `robottypelookup` DISABLE KEYS */;
INSERT INTO `robottypelookup` (`typeID`,`type`) VALUES 
 (1,'LandBot'),
 (2,'AirBot'),
 (3,'SeaBot');
/*!40000 ALTER TABLE `robottypelookup` ENABLE KEYS */;


--
-- Definition of table `targets`
--

 DROP TABLE IF EXISTS `targets`;
 CREATE TABLE `targets` (
  `targetID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `taskedRobot` varchar(10) DEFAULT NULL,
  `timeIdentified` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`targetID`)
--  KEY `FK_targets_1` (`taskedRobot`),
--  CONSTRAINT `FK_targets_1` FOREIGN KEY (`taskedRobot`) REFERENCES `robots` (`ipAddress`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `targets`
--

 /*!40000 ALTER TABLE `targets` DISABLE KEYS */;
 INSERT INTO `targets` (`targetID`,`taskedRobot`,`timeIdentified`) VALUES 
 (12,'22240','2011-04-28 16:35:04');
 /*!40000 ALTER TABLE `targets` ENABLE KEYS */;


--
-- Definition of table `targetlocations`
--

 DROP TABLE IF EXISTS `targetlocations`;
 CREATE TABLE `targetlocations` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `latitude` double NOT NULL,
  `longitude` double NOT NULL,
  `timeUpdated` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `targetcommandcenter_db.targetlocationsID` int(10) unsigned NOT NULL,
  PRIMARY KEY (`ID`) USING BTREE
--  KEY `FK_targetlocations_1` (`targetID`),
--  CONSTRAINT `FK_targetlocations_1` FOREIGN KEY (`targetID`) REFERENCES `targets` (`targetID`)
) ENGINE=InnoDB AUTO_INCREMENT=68 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `targetlocations`
--

/*!40000 ALTER TABLE `targetlocations` DISABLE KEYS */;
 INSERT INTO `targetlocations` (`ID`,`latitude`,`longitude`,`timeUpdated`) VALUES 
 (67,41.39055278,-73.95296944,'2011-04-28 16:35:09');
/*!40000 ALTER TABLE `targetlocations` ENABLE KEYS */;






/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;

