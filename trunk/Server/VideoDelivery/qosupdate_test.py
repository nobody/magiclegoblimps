import qosupdate
from datetime import datetime
import unittest

class qosupdate_test(unittest.TestCase):

    def setUp(self):
        self.timestamp = datetime(2010, 4, 22, 11, 5, 8)

    def testAddSingle(self):
        """
        Adding a single feed.
        """
        server_message = \
"""2010-Apr-22 11:05:08

1;http://127.0.0.1/video.cgi;
"""
        lines = server_message.splitlines()
        t = qosupdate.parse(lines)
        self.assertEqual(t[0], self.timestamp)
        self.assertTrue(len(t[1]) == 1)
        self.assertTrue(len(t[2])== 0)
        

    def testDeleteSingle(self):
        """
        Deleting a single feed.
        """
        server_message = \
"""2010-Apr-22 11:05:08

DELETE 1;http://127.0.0.1/video.cgi;
"""
        lines = server_message.splitlines()
        t = qosupdate.parse(lines)
        self.assertEqual(t[0], self.timestamp)
        self.assertTrue(len(t[1]) == 0)
        self.assertTrue(len(t[2])== 1)

    def testAddSingleNoSemi(self):
        """
        The feed should still get added even if a trailing semicolon is
        missing, only if it is not followed by any object IDs/QoS values.
        """
        server_message = \
"""2010-Apr-22 11:05:08

1;http://127.0.0.1/video.cgi
"""
        lines = server_message.splitlines()
        t = qosupdate.parse(lines)
        self.assertEqual(t[0], self.timestamp)
        self.assertTrue(len(t[1]) == 1)
        self.assertTrue(len(t[2])== 0)

    def testDeleteSingleNoSemi(self):
        """
        A feed should still get deleted even if a trailing semicolon is
        missing.
        """
        server_message = \
"""2010-Apr-22 11:05:08

DELETE 1;http://127.0.0.1/video.cgi
"""
        lines = server_message.splitlines()
        t = qosupdate.parse(lines)
        self.assertEqual(t[0], self.timestamp)
        self.assertTrue(len(t[1]) == 0)
        self.assertTrue(len(t[2])== 1)

    def testMultipleAdd(self):
        """
        Multiple feeds can be added at the same time.
        """
        server_message = \
"""2010-Apr-22 11:05:08

1;http://127.0.0.1/video.cgi;1;0.5;
2;http://127.0.0.2/video.cgi;
3;http://127.0.0.3/video.cgi;2;0.6;3;0.4;
"""
        lines = server_message.splitlines()
        t = qosupdate.parse(lines)
        self.assertEqual(t[0], self.timestamp)
        self.assertTrue(len(t[1]) == 3)
        self.assertTrue(len(t[2]) == 0)
        self.assertEqual(t[1][2].objects, [(2,0.6), (3,0.4)])

    def testMultipleDelete(self):
        """
        Multiple feeds can be deleted at the same time.
        """
        server_message = \
"""2010-Apr-22 11:05:08

DELETE 2;http://127.0.0.2/video.cgi
DELETE 3;http://127.0.0.3/video.cgi
DELETE 1;http://127.0.0.1/video.cgi
"""
        lines = server_message.splitlines()
        t = qosupdate.parse(lines)
        self.assertEqual(t[0], self.timestamp)
        self.assertTrue(len(t[1]) == 0)
        self.assertTrue(len(t[2]) == 3)

    def testMixedAddDelete(self):
        """
        Multiple feeds my be added and/or deleted at the same time.
        """
        server_message = \
"""2010-Apr-22 11:05:08

1;http://127.0.0.1/video.cgi;1;0.5;
DELETE 2;http://127.0.0.2/video.cgi
3;http://127.0.0.3/video.cgi;2;0.6;3;0.4;
"""
        lines = server_message.splitlines()
        t = qosupdate.parse(lines)

    def testNoFeeds(self):
        server_message = \
"""2010-Apr-22 11:05:08

"""
        lines = server_message.splitlines()
        t = qosupdate.parse(lines)
        self.assertEqual(t[0], self.timestamp)
        self.assertTrue(len(t[1]) == 0)
        self.assertTrue(len(t[2]) == 0)

    def testNoTimestamp(self):
        """
        A message without a timestamp should raise an error.
        """
        server_message = \
"""

1;http://127.0.0.1/video.cgi;1;0.5;
"""
        lines = server_message.splitlines()
        try:
            t = qosupdate.parse(lines)
        except Exception as ex:
            self.assertEqual(str(ex), 'could not parse QoS message')

    def testSingleNewLine(self):
        """
        Timestamps must be followed by two newlines or it is an error.
        """
        server_message = \
"""2010-Apr-22 11:05:08
1;http://127.0.0.1/video.cgi;1;0.5;
"""
        lines = server_message.splitlines()
        try:
            t = qosupdate.parse(lines)
        except Exception as ex:
            self.assertEqual(str(ex), 'could not parse QoS message')

    def testMissingQosValue(self):
        """
        Each object ID should have a corresponding QoS value otherwise the
        message is not valid.
        """
        server_message = \
"""2010-Apr-22 11:05:08

1;http://127.0.0.1/video.cgi;1;
"""
        lines = server_message.splitlines()
        t = qosupdate.parse(lines)

    def testMissingLastSemi(self):
        """
        All QoS values should be followed by a semicolon.
        """
        server_message = \
"""2010-Apr-22 11:05:08

1;http://127.0.0.1/video.cgi;1;0.5
"""
        lines = server_message.splitlines()
        t = qosupdate.parse(lines)

    def testGarbageData(self):
        """
        Junk data should be identified by raising an exception.
        """
        server_message = \
"""asldkfj
akfeopwisfjs
asldfkajd
afljewailf jaeslafk
"""
        lines = server_message.splitlines()
        try:
            t = qosupdate.parse(lines)
        except Exception as ex:
            self.assertEqual(str(ex), 'could not parse QoS message')

    def testNoData(self):
        """
        Receiving no data should also raise an exception.
        """
        server_message = ''
        lines = server_message.splitlines()
        try:
            t = qosupdate.parse(lines)
        except Exception as ex:
            self.assertEqual(str(ex), 'could not parse QoS message')

if __name__ == '__main__':
    unittest.main()
