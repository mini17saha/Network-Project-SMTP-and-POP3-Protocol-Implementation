# Network Project: SMTP and POP3 Protocol Implementation
## Overview

This project involves building a sophisticated email system that incorporates both SMTP (Simple Mail Transfer Protocol) and POP3 (Post Office Protocol version 3) protocols. The system allows users to send, receive, and check emails, utilizing these foundational internet standards for email communication. Additionally, user login with username and password is implemented to ensure secure access to the email functionalities.

## Features

SMTP (Simple Mail Transfer Protocol):
-Send emails to any email address.

POP3 (Post Office Protocol version 3):
-Retrieve emails from a mail server.
-Manage email inbox, including reading, and deleting.
-Secure authentication to ensure privacy and data security.

User Login:
-Secure login system requiring username and password.
-User authentication to ensure only authorized access to email functionalities.

## Usage

User Login - user.txt contains the username and password.
Mail format -
    From: <username>@<domain>
    To: <username>@<domain>
    Subject: <Email subject>
    Body
    .
Make sure to end with a period in a newline.
Run smtpmail.c , popserver.c first and then mailclient.c
