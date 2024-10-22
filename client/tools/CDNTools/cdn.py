#!/usr/bin/python
# -*- coding:utf-8 -*-

import sys,os
import urllib, urllib2
import base64
import hmac
import hashlib
from hashlib import sha1
import time
import uuid
import json
from optparse import OptionParser
import ConfigParser
import traceback
import requests

access_key_id = ''
access_key_secret = ''
cdn_url = ''
cdn_server_address = 'http://cdn.aliyuncs.com'

res_server_ip = ''
res_server_port = ''
rsync_user = ''
rsync_password_file = ''
rsync_src_path = ''
rsync_dst_path = ''
rsync_dst_path_ty = ''
rsync_force_all = ''

os.chdir(os.path.abspath(sys.path[0]))
CONFIGSECTION = 'Config'
CONFIGFILE = ""

def setup_config(cdn_file_path):
    global CONFIGFILE
    CONFIGFILE = "%s/cdn.ini"%cdn_file_path

    config = ConfigParser.ConfigParser()

    try:
        config.read(CONFIGFILE)
        global access_key_id, access_key_secret, cdn_url
        global res_server_ip, res_server_port, rsync_user, rsync_password_file, rsync_src_path, rsync_dst_path, rsync_dst_path_ty, rsync_force_all
        access_key_id = config.get(CONFIGSECTION, 'accesskeyid')
        access_key_secret = config.get(CONFIGSECTION, 'accesskeysecret')
        cdn_url = config.get(CONFIGSECTION, 'cdnurl')

        res_server_ip = config.get(CONFIGSECTION, 'resserverip')
        res_server_port = config.get(CONFIGSECTION, 'resserverport')
        rsync_user = config.get(CONFIGSECTION, 'rsyncuser')
        rsync_password_file = config.get(CONFIGSECTION, 'rsyncpasswordfile')
        rsync_src_path = config.get(CONFIGSECTION, 'rsyncsrcpath')
        rsync_dst_path = config.get(CONFIGSECTION, 'rsyncdstpath')
        rsync_dst_path_ty = config.get(CONFIGSECTION, 'rsyncdstpath_ty')
        rsync_force_all = config.get(CONFIGSECTION, 'rsyncforceall') == 'true'

    except Exception, e:
        print traceback.format_exc()
        print("can't get access key pair, use config --id=[accesskeyid] --secret=[accesskeysecret] to setup")
        sys.exit(1)

def get_cdn_url():
    return cdn_url

def get_res_server_info():
    res_server_info = {}
    res_server_info['ip'] = res_server_ip
    res_server_info['port'] = res_server_port
    return res_server_info

def get_rsync_info():
    rsync_info = {}
    rsync_info['user'] = rsync_user
    rsync_info['password_file'] = rsync_password_file
    rsync_info['src_path'] = rsync_src_path
    rsync_info['dst_path'] = rsync_dst_path
    rsync_info['dst_path_ty'] = rsync_dst_path_ty
    rsync_info['force_all'] = rsync_force_all
    return rsync_info

def percent_encode(str):
    # TODO：定位在PHP环境中调用本脚本时，系统环境变量为None的问题
    sys_encoding = sys.stdin.encoding != None and sys.stdin.encoding or 'UTF-8'
    res = urllib.quote(str.decode().encode('utf8'), '')
    res = res.replace('+', '%20')
    res = res.replace('*', '%2A')
    res = res.replace('%7E', '~')
    return res

def compute_signature(parameters, access_key_secret):
    sortedParameters = sorted(parameters.items(), key=lambda parameters: parameters[0])

    canonicalizedQueryString = ''
    for (k,v) in sortedParameters:
        canonicalizedQueryString += '&' + percent_encode(k) + '=' + percent_encode(v)

    stringToSign = 'GET&%2F&' + percent_encode(canonicalizedQueryString[1:])

    h = hmac.new(access_key_secret + "&", stringToSign, sha1)
    signature = base64.encodestring(h.digest()).strip()
    return signature

def compose_url(user_params):
    timestamp = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())

    parameters = { \
            'Format'        : 'JSON', \
            'Version'       : '2014-11-11', \
            'AccessKeyId'   : access_key_id, \
            'SignatureVersion'  : '1.0', \
            'SignatureMethod'   : 'HMAC-SHA1', \
            'SignatureNonce'    : str(uuid.uuid1()), \
            'TimeStamp'         : timestamp, \
    }

    for key in user_params.keys():
        parameters[key] = user_params[key]

    signature = compute_signature(parameters, access_key_secret)
    parameters['Signature'] = signature
    url = cdn_server_address + "/?" + urllib.urlencode(parameters)
    return url

def make_request_url(user_params, quiet=False):
    url = compose_url(user_params)
    return url

def make_request(user_params):
    url = make_request_url(user_params)

    session = requests.Session()
    session.headers = {
        'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/56.0.2924.87 Safari/537.36'
    }

    r = session.get(url)
    result = json.loads(r.text)
    print result
    return result
