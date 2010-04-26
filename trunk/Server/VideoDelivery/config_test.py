import config
import os
import unittest

class config_test(unittest.TestCase):
    
    def setUp(self):
        self.gen = config.Generator()

    def testReplace(self):
        """
        Tests template file generation, which uses all the methods in the
        config.Generator class.
        """
        template_file = '__template.txt'
        output_file = '__output.txt'
        params = {
            'port': '1234',
            'url': 'http://domain.com',
            'name': 'Jordan',
            'date': 'April 25, 2010'
        }
        template_text = """
            Port: $port
            Bit rate: $bit_rate
            Url: $url
            Name: $name, date: $date
            """
        expected_output = """
            Port: 1234
            Bit rate: 200
            Url: http://domain.com
            Name: Jordan, date: April 25, 2010
            """
        with open(template_file, 'w') as f:
            f.write(template_text)
        self.gen.render(params, template_file, output_file)
        with open(output_file, 'r') as f:
            actual_output = f.read()
        self.assertEqual(actual_output, expected_output)
        os.unlink(template_file)
        os.unlink(output_file)

if __name__ == '__main__':
    unittest.main()
