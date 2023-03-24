# Copyright (C) 2015, Wazuh Inc.
#
# This program is free software; you can redistribute it
# and/or modify it under the terms of the GNU General Public
# License (version 2) as published by the FSF - Free Software
# Foundation.


import json
import os
import sys
import time
from socket import socket, AF_UNIX, SOCK_DGRAM

# Exit error codes
ERR_NO_REQUEST_MODULE   = 1
ERR_BAD_ARGUMENTS       = 2
ERR_BAD_MD5_SUM         = 3
ERR_NO_RESPONSE_VT      = 4
ERR_SOCKET_OPERATION    = 5
ERR_FILE_NOT_FOUND      = 6
ERR_INVALID_JSON        = 7

try:
    import requests
    from requests.auth import HTTPBasicAuth
except Exception as e:
    print("No module 'requests' found. Install: pip install requests")
    sys.exit(ERR_NO_REQUEST_MODULE)

# ossec.conf configuration:
# <integration>
#   <name>virustotal</name>
#   <api_key>API_KEY</api_key> <!-- Replace with your VirusTotal API key -->
#   <group>syscheck</group>
#   <alert_format>json</alert_format>
#   <options>JSON</options> <!-- Replace with your custom JSON object -->
# </integration>

# Global vars
debug_enabled   = False
pwd             = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
json_alert      = {}
json_options    = {}

# Log and socket path
LOG_FILE        = f'{pwd}/logs/integrations.log'
SOCKET_ADDR     = f'{pwd}/queue/sockets/queue'

# Constants
ALERT_INDEX     = 1
APIKEY_INDEX    = 2


def main(args: list[str]):
    global debug_enabled
    try:
        # Read arguments
        bad_arguments: bool = False
        if len(args) >= 4:
            msg = '{0} {1} {2} {3} {4}'.format(
                args[1],
                args[2],
                args[3],
                args[4] if len(args) > 4 else '',
                args[5] if len(args) > 5 else ''
            )
            debug_enabled = (len(args) > 4 and args[4] == 'debug')
        else:
            msg = '# ERROR: Wrong arguments'
            bad_arguments = True

        # Logging the call
        with open(LOG_FILE, "a") as f:
            f.write(msg + '\n')

        if bad_arguments:
            debug("# ERROR: Exiting, bad arguments. Inputted: %s" % args)
            sys.exit(ERR_BAD_ARGUMENTS)

        # Core function
        process_args(args)

    except Exception as e:
        debug(str(e))
        raise

def process_args(args: list[str]) -> None:
    """
        This is the core function, creates a message with all valid fields
        and overwrite or add with the optional fields

        Parameters
        ----------
        args : list[str]
            The argument list from main call
    """
    debug("# Running VirusTotal script")

    # Read args
    alert_file_location: str     = args[ALERT_INDEX]
    apikey: str                  = args[APIKEY_INDEX]

    # Load alert. Parse JSON object.
    json_alert  = get_json_alert(alert_file_location)
    debug(f"# Opening alert file at '{alert_file_location}' with '{json_alert}'")

    debug("# Generating message")
    msg: any    = generate_msg(json_alert, json_options,apikey)

    if not msg:
        debug("# ERROR: Empty message")
        raise Exception

    debug(f"# Sending message {msg} from VirusTotal server")
    send_msg(msg, json_alert["agent"])

def debug(msg: str) -> None:
    """
        Log the message in the log file with the timestamp, if debug flag
        is enabled

        Parameters
        ----------
        msg : str
            The message to be logged.
    """
    if debug_enabled:
        print(msg)
        with open(LOG_FILE, "a") as f:
            f.write(msg)

def generate_msg(alert: any, options: any,apikey: str) -> dict[str, str]:
    """
        Generate the JSON object with the message to be send

        Parameters
        ----------
        alert : any
            JSON alert object.
        options: any
            JSON options object.

        Returns
        -------
        msg: str
            The JSON message to send
    """
    alert_output = {}
    # If there is no a md5 checksum present in the alert. Exit.
    if not "md5_after" in alert["syscheck"]:
        debug("# ERROR: Exiting, MD5 checksum not found in alert.")
        sys.exit(ERR_BAD_MD5_SUM)

    # Request info using VirusTotal API
    try:
        vt_response_data = query_api(alert["syscheck"]["md5_after"], apikey)
    except Exception as e:
        debug(str(e))
        sys.exit(ERR_NO_RESPONSE_VT)

    alert_output["virustotal"]                           = {}
    alert_output["integration"]                          = "virustotal"
    alert_output["virustotal"]["found"]                  = 0
    alert_output["virustotal"]["malicious"]              = 0
    alert_output["virustotal"]["source"]                 = {}
    alert_output["virustotal"]["source"]["alert_id"]     = alert["id"]
    alert_output["virustotal"]["source"]["file"]         = alert["syscheck"]["path"]
    alert_output["virustotal"]["source"]["md5"]          = alert["syscheck"]["md5_after"]
    alert_output["virustotal"]["source"]["sha1"]         = alert["syscheck"]["sha1_after"]

    # Check if VirusTotal has any info about the hash
    if vt_response_data['response_code']:
        alert_output["virustotal"]["found"] = 1

    # Info about the file found in VirusTotal
    if alert_output["virustotal"]["found"] == 1:
        if vt_response_data['positives'] > 0:
            alert_output["virustotal"]["malicious"] = 1
        # Populate JSON Output object with VirusTotal request
        alert_output["virustotal"]["sha1"]           = vt_response_data['sha1']
        alert_output["virustotal"]["scan_date"]      = vt_response_data['scan_date']
        alert_output["virustotal"]["positives"]      = vt_response_data['positives']
        alert_output["virustotal"]["total"]          = vt_response_data['total']
        alert_output["virustotal"]["permalink"]      = vt_response_data['permalink']

    return alert_output

def query_api(hash: str, apikey: str) -> any:
    """
        Send a request to VT API and fetch information to build message

        Parameters
        ----------
        hash : str
            Hash need it for parameters
        apikey: str
            JSON options object.

        Returns
        -------
        data: any
            JSON with the response

        Raises
        ------
        Exception
            If the status code is different than 200.
    """
    params    = {'apikey': apikey, 'resource': hash}
    headers   = { "Accept-Encoding": "gzip, deflate", "User-Agent" : "gzip,  Python library-client-VirusTotal" }
    response  = requests.get('https://www.virustotal.com/vtapi/v2/file/report',params=params, headers=headers)

    if response.status_code == 200:
        json_response = response.json()
        vt_response_data = json_response
        return vt_response_data
    else:
        alert_output                  = {}
        alert_output["virustotal"]    = {}
        alert_output["integration"]   = "virustotal"

        if response.status_code == 204:
          alert_output["virustotal"]["error"]         = response.status_code
          alert_output["virustotal"]["description"]   = "Error: Public API request rate limit reached"
          send_msg(json.dumps(alert_output))
          raise Exception("# Error: VirusTotal Public API request rate limit reached")
        elif response.status_code == 403:
          alert_output["virustotal"]["error"]         = response.status_code
          alert_output["virustotal"]["description"]   = "Error: Check credentials"
          send_msg(json.dumps(alert_output))
          raise Exception("# Error: VirusTotal credentials, required privileges error")
        else:
          alert_output["virustotal"]["error"]         = response.status_code
          alert_output["virustotal"]["description"]   = "Error: API request fail"
          send_msg(json.dumps(alert_output))
          raise Exception("# Error: VirusTotal credentials, required privileges error")

def send_msg(msg: any, agent:any = None) -> None:
    if not agent or agent["id"] == "000":
        string      = '1:virustotal:{0}'.format(json.dumps(msg))
    else:
        location    = '[{0}] ({1}) {2}'.format(agent["id"], agent["name"], agent["ip"] if "ip" in agent else "any")
        location    = location.replace("|", "||").replace(":", "|:")
        string      = '1:{0}->virustotal:{1}'.format(location, json.dumps(msg))

    debug("# Final message to send: %s" % string)
    try:
        sock = socket(AF_UNIX, SOCK_DGRAM)
        sock.connect(SOCKET_ADDR)
        sock.send(string.encode())
        sock.close()
    except FileNotFoundError:
        debug(" # Error: Unable to open socket connection at %s" % SOCKET_ADDR)
        sys.exit(ERR_SOCKET_OPERATION)

def get_json_alert(file_location: str) -> any:
    """
        Read JSON alert object from file

        Parameters
        ----------
        file_location : str
            Path to the JSON file location.

        Returns
        -------
        {}: any
            The JSON object read it.

        Raises
        ------
        FileNotFoundError
            If no JSON file is found.
        JSONDecodeError
            If no valid JSON file are used
    """
    try:
        with open(file_location) as alert_file:
            return json.load(alert_file)
    except FileNotFoundError:
        debug("# JSON file for alert %s doesn't exist" % file_location)
        sys.exit(ERR_FILE_NOT_FOUND)
    except json.decoder.JSONDecodeError as e:
        debug("Failed getting JSON alert. Error: %s" % e)
        sys.exit(ERR_INVALID_JSON)

if __name__ == "__main__":
    main(sys.argv)
