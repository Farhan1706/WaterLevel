<?php

// autoload_static.php @generated by Composer

namespace Composer\Autoload;

class ComposerStaticInitcbba8d811a1704876b8f7f4578b5fc68
{
    public static $prefixLengthsPsr4 = array (
        'P' => 
        array (
            'PHPMailer\\PHPMailer\\' => 20,
        ),
    );

    public static $prefixDirsPsr4 = array (
        'PHPMailer\\PHPMailer\\' => 
        array (
            0 => __DIR__ . '/..' . '/phpmailer/phpmailer/src',
        ),
    );

    public static function getInitializer(ClassLoader $loader)
    {
        return \Closure::bind(function () use ($loader) {
            $loader->prefixLengthsPsr4 = ComposerStaticInitcbba8d811a1704876b8f7f4578b5fc68::$prefixLengthsPsr4;
            $loader->prefixDirsPsr4 = ComposerStaticInitcbba8d811a1704876b8f7f4578b5fc68::$prefixDirsPsr4;

        }, null, ClassLoader::class);
    }
}
