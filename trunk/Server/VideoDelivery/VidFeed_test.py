from VidFeed import VidFeed
import unittest

class VidFeed_test(unittest.TestCase):
    
    def setUp(self):
        pass

    def testEquals(self):
        """
        Test the equality comparisons for VidFeed objects.
        """
        vf1 = VidFeed()
        vf2 = VidFeed()
        vf3 = VidFeed()

        vf1.feed_url = '127.0.0.1'
        vf2.feed_url = '127.0.0.1'
        vf3.feed_url = '192.168.1.1'

        self.assertEqual(vf1, vf2)
        self.assertTrue(vf1 == vf2)
        self.assertFalse(vf1 == vf3)

    def testInit(self):
        """
        Ensure proper initialization of VidFeed objects.
        """
        vf1 = VidFeed()
        vf1.feed_url = 'localhost'
        vf1.objects = [(1, 2), (3, 4)]
        vf1.cam_id = 5
        
        vf2 = VidFeed(vf1)
        self.assertEqual(vf1, vf2)
        self.assertEqual(vf1.objects, vf2.objects)
        self.assertEqual(vf1.cam_id, vf2.cam_id)
    
    def testUpdate(self):
        """
        Test that the update method updates the objects array.
        """
        old = [(1,2), (3,4)]
        new = [(1, 0.5)]

        vf1 = VidFeed()
        vf2 = VidFeed()
        vf1.objects = old
        vf2.objects = new
        vf1.update(vf2)

        self.assertEqual(vf2.objects, new)
        self.assertEqual(vf2.objects, vf1.objects)

    def testRepr(self):
        """
        Test that the string representation of a VidFeed object is what we
        expect it to be.
        """
        objects = [(1,0.5), (2,0.7)]
        vf = VidFeed()
        vf.cam_id = 3
        vf.feed_url = '192.168.1.4/video.cgi'
        vf.stream_url = 'http://server/live/stream-3.flv'
        vf.objects = objects
        expect = "VidFeed(cam:3 feed:'192.168.1.4/video.cgi', " + \
            "stream:'http://server/live/stream-3.flv', obj/qos:" + \
            str(objects) + ")"
        self.assertEqual(str(vf), expect)

if __name__ == '__main__':
    unittest.main()
