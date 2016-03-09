--
-- Table structure for table `subscriptions`
--

CREATE TABLE IF NOT EXISTS `subscriptions` (
  `pkid` varchar(40) NOT NULL,
  `roomname` varchar(100) NOT NULL,
  `roomid` varchar(100) NOT NULL,
  `hookid` varchar(100) NOT NULL,
  `fkuser` varchar(40) NOT NULL,
  `shortid` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Triggers `subscriptions`
--
DELIMITER $$
CREATE TRIGGER `t_subscriptions` BEFORE INSERT ON `subscriptions`
 FOR EACH ROW SET new.pkid = uuid()
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `users`
--

CREATE TABLE IF NOT EXISTS `users` (
  `pkid` varchar(40) NOT NULL,
  `usernum` varchar(15) NOT NULL,
  `email` varchar(50) NOT NULL,
  `sparktoken` blob NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Triggers `users`
--
DELIMITER $$
CREATE TRIGGER `t_users` BEFORE INSERT ON `users`
 FOR EACH ROW SET new.pkid = uuid()
$$
DELIMITER ;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `subscriptions`
--
ALTER TABLE `subscriptions`
  ADD PRIMARY KEY (`pkid`);

--
-- Indexes for table `users`
--
ALTER TABLE `users`
  ADD PRIMARY KEY (`pkid`);

--
-- AUTO_INCREMENT for dumped tables
--

