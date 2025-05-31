<?php
require_once 'templates/header.php';

// Get the current page from URL parameter, default to dashboard
$page = $_GET['page'] ?? 'dashboard';

// Load the appropriate content based on the page
switch ($page) {
    case 'dashboard':
        require_once 'templates/dashboard.php';
        break;
    case 'trades':
        require_once 'templates/trades.php';
        break;
    case 'config':
        require_once 'templates/config.php';
        break;
    default:
        require_once 'templates/dashboard.php';
}

require_once 'templates/footer.php'; 